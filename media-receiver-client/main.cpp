//ilyas.cherifialaoui@gmail.com

#include "rtc/rtc.hpp"

#include "parse_cl.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <thread>
#include <unordered_map>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
typedef int SOCKET;

using namespace std::chrono_literals;
using std::shared_ptr;
using std::weak_ptr;
using std::chrono::milliseconds;
using std::chrono::steady_clock;
template <class T> weak_ptr<T> make_weak_ptr(shared_ptr<T> ptr) { return ptr; }

using nlohmann::json;

std::string localId;
std::unordered_map<std::string, shared_ptr<rtc::PeerConnection>> peerConnectionMap;

shared_ptr<rtc::PeerConnection> createPeerConnection(const rtc::Configuration &config,
                                                     weak_ptr<rtc::WebSocket> wws, std::string id);
std::string randomId(size_t length);

// Benchmark
const size_t messageSize = 65535;
rtc::binary messageData(messageSize);

// Benchmark - enableThroughputSet params
bool enableThroughputSet;
int throughtputSetAsKB;
int bufferSize;

int main(int argc, char **argv) 

	try {

		Cmdline params(argc, argv);

		rtc::InitLogger(rtc::LogLevel::Debug);

		// Benchmark - enableThroughputSet params
		enableThroughputSet = params.enableThroughputSet();
		throughtputSetAsKB = params.throughtputSetAsKB();
		bufferSize = params.bufferSize();

		rtc::Configuration config;

		localId = "c1";
		std::cout << "The local ID is " << localId << std::endl;
		auto ws = std::make_shared<rtc::WebSocket>();
		std::promise<void> wsPromise;
		auto wsFuture = wsPromise.get_future();

		auto pc = std::make_shared<rtc::PeerConnection>();

		pc->onStateChange(
			[](rtc::PeerConnection::State state) { 
				std::cout << "State: " << state << std::endl; });

		pc->onGatheringStateChange([pc, &ws](rtc::PeerConnection::GatheringState state) {
			std::cout << "Gathering State: " << state << std::endl;
			if (state == rtc::PeerConnection::GatheringState::Complete) {
				auto description = pc->localDescription();
				json message = {{"id", "c2"},
								{"type", description->typeString()},
								{"sdp", std::string(description.value())}};
				ws->send(message.dump());			
				std::cout << message << std::endl;

			}
		});

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		addr.sin_port = htons(5000);

		rtc::Description::Video media("video", rtc::Description::Direction::RecvOnly);
		media.addH264Codec(96);
		media.setBitrate(
			3000); // Request 3Mbps (Browsers do not encode more than 2.5MBps from a webcam)

		auto track = pc->addTrack(media);

		auto session = std::make_shared<rtc::RtcpReceivingSession>();
		track->setMediaHandler(session);

		track->onMessage(
		    [session, sock, addr](rtc::binary message) {
			    // This is an RTP packet
			    sendto(sock, reinterpret_cast<const char *>(message.data()), int(message.size()), 0,
			           reinterpret_cast<const struct sockaddr *>(&addr), sizeof(addr));
		    },
		    nullptr);

	
		const std::string wsPrefix = params.webSocketServer().find("://") == std::string::npos ? "ws://" : "";
		const std::string url = wsPrefix + params.webSocketServer() + ":" +
								std::to_string(params.webSocketPort()) + "/" + localId;

		std::cout << "WebSocket URL is " << url << std::endl;
		ws->open(url);

		std::cout << "Waiting for signaling to be connected..." << std::endl;

		ws->onOpen([&wsPromise, &ws]() {
			std::cout << "WebSocket connected, signaling ready" << std::endl;
			wsPromise.set_value();
		});

		ws->onError([&wsPromise](std::string s) {
			std::cout << "WebSocket error" << std::endl;
			wsPromise.set_exception(std::make_exception_ptr(std::runtime_error(s)));
		});

		ws->onClosed([]() { std::cout << "WebSocket closed" << std::endl; });
		
		ws->onMessage([&config, wws = make_weak_ptr(ws), pc](auto data) {

			json message = json::parse(std::get<std::string>(data));
			std::cout << "Message recu:" << message << std::endl;

			auto sdp = message["sdp"].get<std::string>();
			rtc::Description answer(message["sdp"].get<std::string>() , message["type"].get<std::string>());

			std::cout << "Use answer : " << message << std::endl;
			pc->setRemoteDescription(answer);

		});
		
		wsFuture.get();
		pc->setLocalDescription();

		std::cout << "Press any key to exit." << std::endl;
		char dummy;
		std::cin >> dummy;

	
		return 0;

	} catch (const std::exception &e) {
		std::cout << "Error: " << e.what() << std::endl;
		peerConnectionMap.clear();
		return -1;
	}