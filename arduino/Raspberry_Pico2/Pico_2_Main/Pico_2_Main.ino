/*
This Code is made for the Raspberrypi pico 2
It is pretty much the same as the code for the teensy certain libraries do not work on the raspberrypi pico 2 due to missing interupt pins these libraries have been changed. 

!The pinout differs from the original!
Also some midi code have changed (LEDs) there are no more overlapping midi code between incomming and outgoing messages. This has made some issues on my testbench
*/

#include <Bounce2.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <ResponsiveAnalogRead.h>
#include <elapsedMillis.h>
#include "PicoEncoder.h"
#include "pio_encoder.h"
#include "Arduino.h"

Adafruit_USBD_MIDI usbMIDI;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usbMIDI, MIDI);
const int eject_pin = 0;
const int track_previous_pin = 1;
const int track_next_pin = 2;
const int search_back_pin = 3;
const int search_forward_pin = 4;
const int cue_pin = 5;
const int play_pin = 7;
const int jet_pin = 6;
const int zip_pin = 8;
const int wah_pin = 9;
const int hold_pin = 10;
const int time_pin = 11;
const int mastertempo_pin = 12;

//LEDs
const int ledCue = 19;
const int ledPlay = 20;
const int ledIntern = 21;
const int ledCd = 22;

//pitch
const int pitchPin = A0;
ResponsiveAnalogRead analog(pitchPin, true);
int lastMSB = -1;
int lastLSB = -1;


//jogwheel
const int jogA_pin = 14;
const int jogB_pin = 15;
const int midiChannel = 2;
const int jogControlNumber = 20;
PioEncoder jog(jogA_pin);
long lastPosition_jog = 0;

//Browse
const int browseA_pin = 16;
const int browseB_pin = 17;
const int load_pin = 18;
const int midiChannelb = 3;
const int DEBOUNCE_MS = 3;  
const int NOTE_SCROLL_DOWN = 70;
const int NOTE_SCROLL_UP = 71;
elapsedMillis debounceTime_browse;  // Temps per filtrar rebots browse¡
PioEncoder browse(browseA_pin);
long lastPosition_browse = 0;


//Debounce of Button
Bounce2::Button cue_Button = Bounce2::Button();
Bounce2::Button play_Button = Bounce2::Button();
Bounce2::Button eject_Button = Bounce2::Button();
Bounce2::Button track_previous_Button = Bounce2::Button();
Bounce2::Button track_next_Button = Bounce2::Button();
Bounce2::Button search_back_Button = Bounce2::Button();
Bounce2::Button search_forward_Button = Bounce2::Button();
Bounce2::Button jet_Button = Bounce2::Button();
Bounce2::Button zip_Button = Bounce2::Button();
Bounce2::Button wah_Button = Bounce2::Button();
Bounce2::Button hold_Button = Bounce2::Button();
Bounce2::Button time_Button = Bounce2::Button();
Bounce2::Button mastertempo_Button = Bounce2::Button();
Bounce2::Button load_Button = Bounce2::Button();
const int channel = 1;

//Notes for incomming midi messages !Not the same as on the original!
const int PLAY_NOTE_INDICATOR = 0x4E;
const int CUE_NOTE_INDICATOR = 0x4c;
const int LEDINTERN_NOTE_INDICATOR = 0x4d;
const int LEDCD_NOTE_INDICATOR = 0x4b;
const int SIESTAPLAY_NOTE_INDICATOR = 0x4a;
bool siestaplay = false;

//LED parameter
bool parpadeig = false;
unsigned long tempsAnterior = 0;



//Handler for LEDs like Play and Cue button to light up
void handleNoteOn(byte channel, byte note, byte velocity) {
  // Comprovem si la nota rebuda és la del nostre indicador de Play
  if (note == PLAY_NOTE_INDICATOR) {
    // Si la velocitat és més gran que 0, vol dir "Play", així que encenem el LED
    if (velocity > 0) {
      digitalWrite(ledPlay, HIGH);

    }
    // Si la velocitat és 0, es tracta com un Note Off, així que l'apaguem
    else {
      digitalWrite(ledPlay, LOW);
    }
  }
  if (note == CUE_NOTE_INDICATOR) {
    // Si la velocitat és més gran que 0, vol dir "Play", així que encenem el LED
    if (velocity > 0) {
      digitalWrite(ledCue, HIGH);
    }
    // Si la velocitat és 0, es tracta com un Note Off, així que l'apaguem
    else {
      digitalWrite(ledCue, LOW);
    }
  }
  //LED INTERN MARCA EL BPM, NOMÉS SI ESTÀ EN PLAY
  if (note == SIESTAPLAY_NOTE_INDICATOR) {
    siestaplay = true;
  }
  if (note == LEDINTERN_NOTE_INDICATOR && siestaplay == true) {
    digitalWrite(ledIntern, HIGH);
  }
  //LED CD el farem parpadejar quan rebi informació que la cançó s'està acabant (mixxx envia 1)
  if (note == LEDCD_NOTE_INDICATOR) {
    parpadeig = (velocity > 0);
  }
}


// Turns LEDs off 
void handleNoteOff(byte channel, byte note, byte velocity) {
  // Comprovem si la nota rebuda és la del nostre indicador de Play
  if (note == PLAY_NOTE_INDICATOR) {
    // Si rebem un Note Off per a aquesta nota, apaguem el LED
    digitalWrite(ledPlay, LOW);
  }
  if (note == CUE_NOTE_INDICATOR) {
    // Si rebem un Note Off per a aquesta nota, apaguem el LED
    digitalWrite(ledCue, LOW);
  }

  if (note == LEDCD_NOTE_INDICATOR) {
    parpadeig = false;
  }

  if (note == SIESTAPLAY_NOTE_INDICATOR) {
    siestaplay = false;
  }
  if (note == LEDINTERN_NOTE_INDICATOR || siestaplay == false) {
    digitalWrite(ledIntern, LOW);
  }
}

// Initializing debouncing of Buttons
void setupBounce(Bounce2::Button &button, int Pin) {
  button.attach(Pin, INPUT_PULLUP);
  button.interval(10);
  button.setPressedState(HIGH);
}

void setup() {
  usbMIDI.begin(0);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  setupBounce(play_Button, play_pin);
  setupBounce(cue_Button, cue_pin);
  setupBounce(eject_Button, eject_pin);
  setupBounce(track_previous_Button, track_previous_pin);
  setupBounce(track_next_Button, track_next_pin);
  setupBounce(search_back_Button, search_back_pin);
  setupBounce(search_forward_Button, search_forward_pin);
  setupBounce(jet_Button, jet_pin);
  setupBounce(zip_Button, zip_pin);
  setupBounce(wah_Button, wah_pin);
  setupBounce(hold_Button, hold_pin);
  setupBounce(time_Button, time_pin);
  setupBounce(mastertempo_Button, mastertempo_pin);
  setupBounce(load_Button, load_pin);

  pinMode(ledCue, OUTPUT);
  pinMode(ledPlay, OUTPUT);
  pinMode(ledIntern, OUTPUT);
  pinMode(ledCd, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);

  jog.begin();
  browse.begin();
}
// Decodes Jogwheel
void jogread() {
  long newPosition_jog = jog.getCount();
  if (newPosition_jog % 4 == 0 && newPosition_jog != lastPosition_jog) {
    if (newPosition_jog > lastPosition_jog) {
      MIDI.sendControlChange(jogControlNumber, 65, midiChannel);
    } else {
      MIDI.sendControlChange(jogControlNumber, 63, midiChannel);
    }
    lastPosition_jog = newPosition_jog;
  }
}

// This method decodes the Browse only the rotation is handled here for the Load knob have a look at the other knobs
void browseread() {
  long newPosition_browse = browse.getCount();
  long delta_browse = newPosition_browse - lastPosition_browse;

  if (debounceTime_browse > DEBOUNCE_MS && delta_browse != 0) {
    if (newPosition_browse % 4 == 0 && newPosition_browse != lastPosition_browse) {

      if (newPosition_browse > lastPosition_browse) {
        // Direcció: DRETA (baixar a la llista)
        MIDI.sendNoteOn(NOTE_SCROLL_DOWN, 127, midiChannelb);
        MIDI.sendNoteOff(NOTE_SCROLL_DOWN, 0, midiChannelb);

      } else {
        MIDI.sendNoteOn(NOTE_SCROLL_UP, 127, midiChannelb);
        MIDI.sendNoteOff(NOTE_SCROLL_UP, 0, midiChannelb);
      }   
      lastPosition_browse = newPosition_browse;
    }
  }
}


void updateButtons() {
  cue_Button.update();
  play_Button.update();
  eject_Button.update();
  track_previous_Button.update();
  track_next_Button.update();
  search_back_Button.update();
  search_forward_Button.update();
  jet_Button.update();
  zip_Button.update();
  wah_Button.update();
  hold_Button.update();
  time_Button.update();
  mastertempo_Button.update();
  load_Button.update();

  while (!TinyUSBDevice.mounted()) delay(1);
}

void loop() {
  updateButtons();

//Button triggering
  //Play
  if (play_Button.released()) {
    MIDI.sendNoteOn(60, 127, channel);
  } else if (play_Button.pressed()) {
    MIDI.sendNoteOn(60, 0, channel);
  }

  //cue
  if (cue_Button.released()) {
    MIDI.sendNoteOn(61, 127, channel);
  } else if (cue_Button.pressed()) {
    MIDI.sendNoteOn(61, 0, channel);
  }

  //mastertempo
  if (mastertempo_Button.released()) {
    MIDI.sendNoteOn(62, 127, channel);
  } else if (mastertempo_Button.pressed()) {
    MIDI.sendNoteOn(62, 0, channel);
  }

  //Eject
  if (eject_Button.released()) {
    MIDI.sendNoteOn(63, 127, channel);
  } else if (eject_Button.pressed()) {
    MIDI.sendNoteOn(63, 0, channel);
  }

  //trackprev
  if (track_previous_Button.released()) {
    MIDI.sendNoteOn(64, 127, channel);
  } else if (track_previous_Button.pressed()) {
    MIDI.sendNoteOn(64, 0, channel);
  }

  //nextTrack
  if (track_next_Button.released()) {
    MIDI.sendNoteOn(65, 127, channel);
  } else if (track_next_Button.pressed()) {
    MIDI.sendNoteOn(65, 0, channel);
  }

  //searchback
  if (search_back_Button.released()) {
    MIDI.sendNoteOn(66, 127, channel);
  } else if (search_back_Button.pressed()) {
    MIDI.sendNoteOn(66, 0, channel);
  }

  //searchforward
  if (search_forward_Button.released()) {
    MIDI.sendNoteOn(67, 127, channel);
  } else if (search_forward_Button.pressed()) {
    MIDI.sendNoteOn(67, 0, channel);
  }

  //jet
  if (jet_Button.released()) {
    MIDI.sendNoteOn(68, 127, channel);
  } else if (jet_Button.pressed()) {
    MIDI.sendNoteOn(68, 0, channel);
  }

  //zip
  if (zip_Button.released()) {
    MIDI.sendNoteOn(69, 127, channel);
  } else if (zip_Button.pressed()) {
    MIDI.sendNoteOn(69, 0, channel);
  }

  //wah
  if (wah_Button.released()) {
    MIDI.sendNoteOn(70, 127, channel);
  } else if (wah_Button.pressed()) {
    MIDI.sendNoteOn(70, 0, channel);
  }

  //hold
  if (hold_Button.released()) {
    MIDI.sendNoteOn(71, 127, channel);
  } else if (hold_Button.pressed()) {
    MIDI.sendNoteOn(71, 0, channel);
  }

  //time
  if (time_Button.released()) {
    MIDI.sendNoteOn(72, 127, channel);
  } else if (time_Button.pressed()) {
    MIDI.sendNoteOn(72, 0, channel);
  }

  //load
  if (load_Button.released()) {
    MIDI.sendNoteOn(73, 127, channel);
  } else if (load_Button.pressed()) {
    MIDI.sendNoteOn(73, 0, channel);
  }


  //pitch
  analog.update();
  int raw = analog.getValue();  // valor ja suavitzat

  int value14 = map(raw, 0, 1023, 0, 16383);

  byte msb = (value14 >> 7) & 0x7F;
  byte lsb = value14 & 0x7F;


  if (msb != lastMSB) {
    MIDI.sendControlChange(0, msb, 1);
    lastMSB = msb;
  }

  if (lsb != lastLSB) {
    MIDI.sendControlChange(32, lsb, 1);
    lastLSB = lsb;
  }

  //jog
  jogread();
  browseread();
  if (parpadeig) {
    if (millis() - tempsAnterior >= 1000) {
      tempsAnterior = millis();
      digitalWrite(ledCd, !digitalRead(ledCd));
    }
  } else {
    digitalWrite(ledCd, LOW);
  }


// Read incomming midi messages
  while (MIDI.read()) {}
}
