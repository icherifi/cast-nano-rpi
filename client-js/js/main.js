//ilyas.cherifialaoui@gmail.com

'use strict';

const startButton = document.getElementById('startButton');
const hangupButton = document.getElementById('hangupButton');

let signalingChannel = null

hangupButton.disabled = true;
startButton.addEventListener('click', start);
hangupButton.addEventListener('click', hangup);

let startTime;
const localVideo = document.getElementById('localVideo');
const remoteVideo = document.getElementById('remoteVideo');

localVideo.addEventListener('loadedmetadata', function () {
  console.log(`Local video videoWidth: ${this.videoWidth}px,  videoHeight: ${this.videoHeight}px`);
});

const offerOptions = {
  offerToReceiveAudio: 1,
  offerToReceiveVideo: 1
};
let pc;

async function start() {
  
  // Set up an asynchronous communication channel that will be
  // used during the peer connection setup'
  startButton.disabled = true;
  hangupButton.disabled = false;
  signalingChannel = new WebSocket('ws://localhost:8000/c2');
  signalingChannel.addEventListener('message', async message => {
    const parsedMessage = JSON.parse(message.data);
    console.log(`Message from server: ${message.data}`);

    if (parsedMessage.type === "offer") {
      const offer = parsedMessage;
      pc = new RTCPeerConnection({
        bundlePolicy: 'max-bundle',
      });
      pc.onicecandidate = (event) => {
        if (event.candidate) {
          console.log('Got ICE candidate:', event.candidate);
          // Candidate is ready.
        } else {
          console.log('ICE gathering finished');
          // All ICE candidates have been gathered
        }
      };
  
      pc.oniceconnectionstatechange = (state) => {
        console.log('ICE connection state changed:', pc.iceConnectionState);
        // Handle ICE connection state changes
        if ( pc.iceConnectionState == "connected") {
          const answerToSend = {
            id: "c1",
            type: pc.localDescription.type,
            sdp: pc.localDescription.sdp,
          };
          console.log('Sending answer:', answerToSend);
          signalingChannel.send(JSON.stringify(answerToSend));
        }

      };

      await pc.setRemoteDescription(offer);

      await navigator.mediaDevices.getDisplayMedia(
        {audio: true, video: true}
      )
          .then(handleSuccess, handleError);
    
      const answer = await pc.createAnswer();
      await pc.setLocalDescription(answer);
    }
  });
  
  signalingChannel.onopen = () => {
    const message = {
      id: "c1",
      type: "candidate",
      info: "I am a client waiting for streaming."
    };
    signalingChannel.send(JSON.stringify(message));
    console.log('WebSocket connection established');
  };
  
  signalingChannel.onclose = () => {
    console.log('WebSocket closed');
  };
  
}

async function onIceCandidate(pc, event) {
  try {
    await (getOtherPc(pc).addIceCandidate(event.candidate));
    onAddIceCandidateSuccess(pc);
  } catch (e) {
    onAddIceCandidateError(pc, e);
  }
  console.log(`${getName(pc)} ICE candidate:\n${event.candidate ? event.candidate.candidate : '(null)'}`);
}

function onAddIceCandidateSuccess(pc) {
  console.log(`${getName(pc)} addIceCandidate success`);
}

function onAddIceCandidateError(pc, error) {
  console.log(`${getName(pc)} failed to add ICE Candidate: ${error.toString()}`);
}

function onIceStateChange(pc, event) {
  if (pc) {
    console.log(`${getName(pc)} ICE state: ${pc.iceConnectionState}`);
    console.log('ICE state change event: ', event);
  }
}

function handleSuccess(media) {
  startButton.disabled = true;
  const video = document.querySelector('video');
  video.srcObject = media;
  media.getTracks().forEach(track => pc.addTrack(track, media));
  console.log('Got media stream:', media);
}

function handleError(error) {
  console.log(`getDisplayMedia error: ${error.name}`, error);
}

function hangup() {
  console.log('Ending call');
  
  // Stop all tracks
  if (pc) {
    pc.getSenders().forEach(sender => sender.track.stop());
  }
  // Close web socket
  signalingChannel.close();
  
  pc.close();
  pc = null;
  
  localVideo.srcObject = null;
  hangupButton.disabled = true;
  startButton.disabled = false;
}
