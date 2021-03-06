/* Copyright (C) 2013-2014 Michal Brzozowski (rusolis@poczta.fm)

   This file is part of KeeperRL.

   KeeperRL is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   KeeperRL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.
   If not, see http://www.gnu.org/licenses/ . */

#include "stdafx.h"
#include "music.h"
#include "options.h"

using sf::Music;

Jukebox::Jukebox(Options* options) {
  music.reset(new Music[20]);
  on = false;
  options->addTrigger(OptionId::MUSIC, [this](bool turnOn) { if (turnOn != on) toggle(); });
}

void Jukebox::addTrack(MusicType type, const string& path) {
  music[numTracks].openFromFile(path);
  byType[type].push_back(numTracks);
  ++numTracks;
}

void Jukebox::toggle() {
  if (!numTracks)
    return;
  if ((on = !on)) {
    current = chooseRandom(byType[getCurrentType()]);
    currentPlaying = current;
    music[current].play();
  } else
    music[current].stop();
}

void Jukebox::setCurrent(MusicType c) {
  current = chooseRandom(byType[c]);
}

void Jukebox::continueCurrent() {
  if (byType[getCurrentType()].size() >= 2)
    while (current == currentPlaying)
      setCurrent(getCurrentType());
}

MusicType Jukebox::getCurrentType() {
  for (auto& elem : byType)
    if (contains(elem.second, current))
      return elem.first;
  FAIL << "Track type not found " << current;
  return MusicType::PEACEFUL;
}

const int volumeDec = 20;

void Jukebox::update(MusicType c) {
  if (byType[c].empty())
    return;
  if (getCurrentType() != c)
    current = chooseRandom(byType[c]);
  if (!on)
    return;
  if (current != currentPlaying) {
    if (music[currentPlaying].getVolume() == 0) {
      music[currentPlaying].stop();
      currentPlaying = current;
      music[currentPlaying].setVolume(100);
      music[currentPlaying].play();
    } else
      music[currentPlaying].setVolume(max(0.0f, music[currentPlaying].getVolume() - volumeDec));
  } else
  if (music[current].getStatus() == sf::SoundSource::Stopped) {
    continueCurrent();
    currentPlaying = current;
    music[current].play();
  }
}
