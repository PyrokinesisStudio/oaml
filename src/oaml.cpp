#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinyxml2.h"
#include "oamlCommon.h"

oamlData::oamlData() {
	debug = 0;
	tracksN = 0;

	curTrack = NULL;

	dbuffer = NULL;

	freq = 0;
	channels = 0;
	bytesPerSample = 0;

	volume = 255;

	timeMs = 0;
	tension = 0;
	tensionMs = 0;
}

oamlData::~oamlData() {
}

int oamlData::ReadDefs(const char *filename, const char *path) {
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile(filename) != tinyxml2::XML_NO_ERROR)
		return -1;

	tinyxml2::XMLElement *el = doc.FirstChildElement("track");
	while (el != NULL) {
		oamlTrack *track = new oamlTrack();

		tinyxml2::XMLElement *el2 = el->FirstChildElement();
		while (el2 != NULL) {
			if (strcmp(el2->Name(), "name") == 0) track->SetName(el2->GetText());
			else if (strcmp(el2->Name(), "fadeIn") == 0) track->SetFadeIn(strtol(el2->GetText(), NULL, 0));
			else if (strcmp(el2->Name(), "fadeOut") == 0) track->SetFadeOut(strtol(el2->GetText(), NULL, 0));
			else if (strcmp(el2->Name(), "xfadeIn") == 0) track->SetXFadeIn(strtol(el2->GetText(), NULL, 0));
			else if (strcmp(el2->Name(), "xfadeOut") == 0) track->SetXFadeOut(strtol(el2->GetText(), NULL, 0));
			else if (strcmp(el2->Name(), "audio") == 0) {
				oamlAudio *audio = new oamlAudio();

				tinyxml2::XMLElement *el3 = el2->FirstChildElement();
				while (el3 != NULL) {
					if (strcmp(el3->Name(), "filename") == 0) {
						char fname[1024];
						sprintf(fname, "%s%s", path, el3->GetText());
						audio->SetFilename(fname);
					}
					else if (strcmp(el3->Name(), "type") == 0) audio->SetType(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "bars") == 0) audio->SetBars(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "bpm") == 0) audio->SetBPM(strtof(el3->GetText(), NULL));
					else if (strcmp(el3->Name(), "beatsPerBar") == 0) audio->SetBeatsPerBar(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "fadeIn") == 0) audio->SetFadeIn(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "fadeOut") == 0) audio->SetFadeOut(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "condId") == 0) audio->SetCondId(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "condType") == 0) audio->SetCondType(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "condValue") == 0) audio->SetCondValue(strtol(el3->GetText(), NULL, 0));
					else if (strcmp(el3->Name(), "condValue2") == 0) audio->SetCondValue2(strtol(el3->GetText(), NULL, 0));

					el3 = el3->NextSiblingElement();
				}

				track->AddAudio(audio);
			}

			el2 = el2->NextSiblingElement();
		}

		tracks[tracksN++] = track;

		el = el->NextSiblingElement();
	}

	return 0;
}

int oamlData::Init(const char *pathToMusic) {
	char path[1024];
	char filename[1024];

	ASSERT(pathToMusic != NULL);

	int len = strlen(pathToMusic);
	if (len > 0 && (pathToMusic[len-1] == '/')) {
		sprintf(path, "%s", pathToMusic);
	} else {
		sprintf(path, "%s/", pathToMusic);
	}

	sprintf(filename, "%soaml.defs", path);
	if (ReadDefs(filename, path) == -1)
		return -1;

	if (debug) {
		dbuffer = new ByteBuffer();
	}

	return 0;
}

void oamlData::SetAudioFormat(int audioFreq, int audioChannels, int audioBytesPerSample) {
	freq = audioFreq;
	channels = audioChannels;
	bytesPerSample = audioBytesPerSample;
}

int oamlData::PlayTrackId(int id) {
	if (id >= tracksN)
		return -1;

	if (curTrack) {
		curTrack->Stop();
	}

	curTrack = tracks[id];
	curTrack->Play();

	return 0;
}

int oamlData::PlayTrack(const char *name) {
	ASSERT(name != NULL);

//	printf("%s %s\n", __FUNCTION__, name);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return PlayTrackId(i);
		}
	}

	return -1;
}

bool oamlData::IsTrackPlaying(const char *name) {
	ASSERT(name != NULL);

	for (int i=0; i<tracksN; i++) {
		if (strcmp(tracks[i]->GetName(), name) == 0) {
			return IsTrackPlayingId(i);
		}
	}

	return false;
}

bool oamlData::IsTrackPlayingId(int id) {
	if (id >= tracksN)
		return false;

	return tracks[id]->IsPlaying();
}

bool oamlData::IsPlaying() {
	for (int i=0; i<tracksN; i++) {
		if (tracks[i]->IsPlaying())
			return true;
	}

	return false;
}

void oamlData::StopPlaying() {
	for (int i=0; i<tracksN; i++) {
		if (tracks[i]->IsPlaying()) {
			tracks[i]->Stop();
			break;
		}
	}
}

void oamlData::MixToBuffer(void *buffer, int size) {
	ASSERT(buffer != NULL);
	ASSERT(size != 0);

	for (int i=0; i<size; i++) {
		int sample = 0;

		for (int j=0; j<tracksN; j++) {
			sample+= tracks[j]->Read32();
		}

		sample>>= 16;
		sample = (sample * volume) / OAML_VOLUME_MAX;

		if (sample > 32767) sample = 32767;
		if (sample < -32768) sample = -32768;

		if (bytesPerSample == 2) {
			short *sbuf = (short*)buffer;
			sbuf[i]+= (short)sample;
		}
	}
}

void oamlData::SetCondition(int id, int value) {
//	printf("%s %d %d\n", __FUNCTION__, id, value);
	if (curTrack) {
		curTrack->SetCondition(id, value);
	}
}

void oamlData::SetVolume(int vol) {
	volume = vol;

	if (volume < OAML_VOLUME_MIN) volume = OAML_VOLUME_MIN;
	if (volume > OAML_VOLUME_MAX) volume = OAML_VOLUME_MAX;
}

void oamlData::AddTension(int value) {
	tension+= value;
	if (tension >= 100) {
		tension = 100;
	}
}

void oamlData::Update() {
	uint64_t ms = GetTimeMs64();

	// Update each second
	if (ms >= (timeMs + 1000)) {
//		printf("%s %d %lld %d\n", __FUNCTION__, tension, tensionMs - ms, ms >= (tensionMs + 5000));
		// Don't allow sudden changes of tension after it changed back to 0
		if (tension > 0) {
			SetCondition(1, tension);
			tensionMs = ms;
		} else {
			if (ms >= (tensionMs + 5000)) {
				SetCondition(1, tension);
				tensionMs = ms;
			}
		}

		// Lower tension
		if (tension >= 1) {
			tension-= (tension+20)/10;
			if (tension < 0)
				tension = 0;
		}

		timeMs = ms;
	}

/*	if (buffer <= 4096) {
		track->Read(buffer);
	}*/
}

void oamlData::Shutdown() {
//	printf("%s\n", __FUNCTION__);
	if (dbuffer) {
		wavWriteToFile("out.wav", dbuffer, 2, 44100, 2);
	}
}
