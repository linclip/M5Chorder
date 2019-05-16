#include <M5Stack.h>
#include <M5TreeView.h>
#include <vector>
#include "M5StackUpdater.h"
#include "MenuItemKey.h"
#include "MenuItemScale.h"
#include "BLEMidi.h"
#include "Chord.h"
#include "Scale.h"

#define DEVICE_NAME "BLEChorder"

std::vector<uint8_t> playingNotes;
Scale scale = Scale(0);
Chord IM7 = scale.degreeToChord(0,0,Chord(Chord::C,Chord::MajorSeventh));
Chord IVM7 = scale.degreeToChord(3,0,Chord(Chord::C,Chord::MajorSeventh));
Chord V7 =  scale.degreeToChord(4,0,Chord(Chord::C,Chord::Seventh));

M5TreeView tv;
typedef std::vector<MenuItem*> vmi;
M5ButtonDrawer buttonDrawer;

//画面
enum Scene : uint8_t {
  Connection,
  Play,
  Function,
  length
};

Scene currentScene = Scene::length; //初回changeSceneにて正しく描画するためConnection以外を指定
Scene requiredToChangeScene;

void changeScene(Scene scene) {
  //描画処理をloopの最後で実行するために一旦別の変数に退避させる(フラグを立てる)
  requiredToChangeScene = scene;
}

//setup()とloop()の最後にて実行　それ以外では触らない！
void _changeScene_raw() {
  M5.Lcd.clear();

  //終了処理
  switch(currentScene) {
  }

  //開始処理
  switch(requiredToChangeScene) {
    case Scene::Connection:
      M5.Lcd.setCursor(0, 48);
      M5.Lcd.setTextSize(4);
      M5.Lcd.println("BLEChorder");
      M5.Lcd.setTextSize(2);
      M5.Lcd.println("BLE MIDI Chordpad App\n");
      M5.Lcd.setTextSize(1);
      M5.Lcd.println("Advertising...");
      M5.Lcd.println("Press the button A to power off.");
    break;
    case Scene::Play:
      M5.Lcd.setCursor(0,0);
      M5.Lcd.setTextSize(2);
      M5.Lcd.println(scale.toString());
      buttonDrawer.setText("IM7","IVM7","V7/Menu");
      buttonDrawer.draw(true);
    break;
    case Scene::Function:
      tv.begin();
    break;
  }
  currentScene = requiredToChangeScene;
}

void sendNotes(bool isNoteOn, std::vector<uint8_t> notes, int vel) {
  if(isNoteOn) {
    for(uint8_t n : notes) {
      Midi.sendNote(0x90, n, vel);
    }
    playingNotes.insert(playingNotes.end(),notes.begin(),notes.end());
  }
  else {
    for(uint8_t n : playingNotes) {
      Midi.sendNote(0x80, n, 0);
    }
    playingNotes.clear();
  }
}

void playChord(Chord chord) {
  sendNotes(true,chord.toMidiNoteNumbers(64,16),120);
  M5.Lcd.setTextSize(4);
  M5.Lcd.fillRect(0,60,320,120,BLACK);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextSize(5);
  M5.Lcd.drawString(chord.toString(), 160, 120, 2);
}

class ServerCallbacks: public BLEMidiServerCallbacks {
    void onConnect(BLEServer* pServer) {
      BLEMidiServerCallbacks::onConnect(pServer);
      changeScene(Scene::Play);
    };

    void onDisconnect(BLEServer* pServer) {
      BLEMidiServerCallbacks::onDisconnect(pServer);
      changeScene(Scene::Connection);
    }
};

void callBackKey(MenuItem* sender) {
  MenuItemKey* mi((MenuItemKey*)sender);
  scale.key = mi->value;
  IM7 = scale.degreeToChord(0,0,Chord(Chord::C,Chord::MajorSeventh));
  IVM7 = scale.degreeToChord(3,0,Chord(Chord::C,Chord::MajorSeventh));
  V7 =  scale.degreeToChord(4,0,Chord(Chord::C,Chord::Seventh));
}

void callBackScale(MenuItem* sender) {
  MenuItemKey* mi((MenuItemKey*)sender);
  scale.currentScale = Scale::getAvailableScales()[mi->value].get();
  IM7 = scale.degreeToChord(0,0,Chord(Chord::C,Chord::MajorSeventh));
  IVM7 = scale.degreeToChord(3,0,Chord(Chord::C,Chord::MajorSeventh));
  V7 =  scale.degreeToChord(4,0,Chord(Chord::C,Chord::Seventh));
}

void setup() {
  M5.begin();
  Wire.begin();
  Serial.begin(9600);

  //SD Updater
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }

  changeScene(Scene::Connection);
  _changeScene_raw();

  Midi.begin(DEVICE_NAME, new ServerCallbacks(), NULL);

  //FunctionMenu
  tv.setItems(vmi{
    new MenuItemKey("Key", scale.key, callBackKey),
    new MenuItemScale("Scale", 0, callBackScale)
  });
  M5ButtonDrawer::width = 106;
  tv.clientRect.x = 2;
  tv.clientRect.y = 16;
  tv.clientRect.w = 196;
  tv.clientRect.h = 200;
  tv.itemWidth = 176;
  tv.useFACES       = true;
  tv.useCardKB      = true;
  tv.useJoyStick    = true;
  tv.usePLUSEncoder = true;
}

void loop() {
  switch(currentScene) {
    case Scene::Connection:
      M5.update();
      if(M5.BtnA.wasPressed()) M5.Power.deepSleep();
    break;
    case Scene::Play:
      M5.update();
      if(M5.BtnC.pressedFor(1000)) {changeScene(Scene::Function); break;}
      if(M5.BtnA.wasPressed())  playChord(IM7);
      if(M5.BtnA.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      if(M5.BtnB.wasPressed())  playChord(IVM7);
      if(M5.BtnB.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      if(M5.BtnC.wasPressed())  playChord(V7);
      if(M5.BtnC.wasReleased()) sendNotes(false,std::vector<uint8_t>(),120);
      buttonDrawer.draw();
    break;
    case Scene::Function:
      tv.update();
      if(M5.BtnA.wasReleased() && (M5TreeView::getFocusItem()->parentItem() == &tv)) changeScene(Scene::Play);
    break;
  }
  if(currentScene != requiredToChangeScene) _changeScene_raw();
}