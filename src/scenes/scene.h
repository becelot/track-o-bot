#pragma once

#include "../dhash.h"
#include "../hearthstone.h"
#include "../generated_markers.h"

#include <QPixmap>
#include <QElapsedTimer>

#define ADD_GENERATED_MARKER(NAME, DEF) AddMarker(NAME, DEF##_PATH, DEF##_X, DEF##_Y, DEF##_WIDTH, DEF##_HEIGHT)

class Scene
{
private:
  class Marker {
  public:
    string name;
    dhash hash;
    int x, y, w, h; // virtual canvas attributes!

    Marker():name(""), x(0), y(0), w(0), h(0) {
    }

    Marker(const string& name, const string& templateImagePath, int x, int y, int w, int h)
      :name(name), x(x), y(y), w(w), h(h)
    {
      hash = dhash_for_pixmap(QPixmap(templateImagePath.c_str()));
    }
  };

  map<string, Marker> markers;
  string name;

  map<string, QElapsedTimer> timerForMarkers;

public:
  Scene(const string& name):name(name) {
    Init();
  }

  virtual ~Scene() {
  }

  void AddMarker(const string& name, const string& templateImagePath, int x, int y, int w, int h) {
    markers[name] = Marker(name, templateImagePath, x, y, w, h);
  }

  bool FindMarker(const string& name, qint64 findIntervalInMs = 300) {
    const Marker& marker = markers[name];
    const QPixmap& capture = Hearthstone::Instance()->Capture(marker.x, marker.y, marker.w, marker.h);
    dhash currentHash = dhash_for_pixmap(capture);
    bool similar = dhash_check_similarity(currentHash, marker.hash);

    // Due to noise and the resulting false positives
    // make sure the marker is found for a certain amount of ms
    // to ensure the marker is indeed the one we looked for
    QElapsedTimer& timerForMarker = timerForMarkers[name];

    if(similar) {
      if(!timerForMarker.isValid()) {
        timerForMarker.start();
      }
    } else {
      timerForMarker.invalidate();
    }

    return timerForMarker.isValid() && timerForMarker.hasExpired(findIntervalInMs);
  }

  virtual bool Active() = 0;

  virtual void Init() {
  }

  virtual void Update() {
  }

  const string& GetName() const {
    return name;
  }
};
