#ifndef __DRUM_SAMPLER__
#define __DRUM_SAMPLER__

#include "axLib.h"
#include <vector>
#include <string>
#include <cmath>

#include "axAudio.h"
#include "AudioWaveform.h"

class SynthControl : public axPanel
{
public:
	SynthControl(axWindow* parent,
				 const axRect& rect, 
				 const string& snd_path,
				 Audio* audio,
				 const int& track_num);

	axEVENT_ACCESSOR(axButtonMsg, OnOpenFile);
	axEVENT(axKnobMsg, OnFilterFrequency);
	axEVENT(axKnobMsg, OnFilterRes);
	axEVENT(axKnobMsg, OnFilterEnvelope);
	axEVENT(axKnobMsg, OnAttack);
	axEVENT(axKnobMsg, OnDecay);
	axEVENT(axKnobMsg, OnGain);
	axEVENT(axKnobMsg, OnTuning);
	axEVENT(axKnobMsg, OnTuningEnv);

private:
	// Events.
	axImage* _bgImg;
	AudioMidiSeq* _audio;
	int _trackNum;
	axWaveform* _wave;

	void OnOpenFile(const axButtonMsg& msg);
	void OnFilterFrequency(const axKnobMsg& msg);
	void OnFilterRes(const axKnobMsg& msg);
	void OnFilterEnvelope(const axKnobMsg& msg);
	void OnAttack(const axKnobMsg& msg);
	void OnDecay(const axKnobMsg& msg);
	void OnGain(const axKnobMsg& msg);
	void OnTuning(const axKnobMsg& msg);
	void OnTuningEnv(const axKnobMsg& msg);
	virtual void OnPaint();


};

class DrumPad;

class DrumPadMsg
{
public:
	DrumPadMsg()
	{
		_sender = nullptr;
	}

	DrumPadMsg(DrumPad* sender, const int& msg)
	{
		_sender = sender;
		_msg = msg;
	}

	DrumPad* GetSender() const
	{
		return _sender;
	}

	int GetMsg() const
	{
		return _msg;
	}

private:
	DrumPad* _sender;
	int _msg;
};

class DrumPad : public axPanel
{
public:
	DrumPad(axWindow* parent,
		const axRect& rect, 
		const int& track_id,
		axEvtFunction(DrumPadMsg) click_evt);

	void SetTrackName(const string& name);
	void SetSelected();
	void SetUnselected();

	//axEVENT(axButtonMsg, OnOpenFile);

private:
	axImage* _bgImg;
	int _trackId;
	string _trackName;
	bool _highlight, _selected;

	axEvtFunction(DrumPadMsg) _clickEvent;
	// Events.

	//void OnOpenFile(const axButtonMsg& msg);
	virtual void OnMouseLeftDown(const axPoint& pos);
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
	virtual void OnPaint();
};

class DrumSampler : public axPanel
{
public:
	DrumSampler(axWindow* parent,
		const axRect& rect, Audio* audio);

	//axEVENT(axButtonMsg, OnOpenFile);

	axEVENT(DrumPadMsg, OnDrumPadClick);

private:
	AudioMidiSeq* _audio;
	vector<SynthControl*> _synths;
	vector<DrumPad*> _pads;

	axImage* _topBar;

	
	//function<void(axButtonMsg)> button_click;

	// Events.

	void OnDrumPadClick(const DrumPadMsg& msg);
	virtual void OnPaint();
};


#endif // __DRUM_SAMPLER__