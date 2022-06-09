#include "plugin.hpp"
  
// This is my attempt at creating
struct VCVSliderTall : app::SvgSlider {
	VCVSliderTall() {
		setBackgroundSvg(Svg::load(asset::plugin(pluginInstance, "res/VCVSliderTall.svg")));
		setHandleSvg(Svg::load(asset::system("res/ComponentLibrary/VCVSliderHandle.svg")));
		setHandlePosCentered(
			math::Vec(19.84260/2, 150.535 - 11.74218/2),
			math::Vec(19.84260/2, 0.0 + 11.74218/2)
		);
	}
};

template <typename TLightBase = RedLight>
struct VCVLightSliderTall : LightSlider<VCVSliderTall, VCVSliderLight<TLightBase>> {
	VCVLightSlider() {}
};

struct _16n : Module {
	enum ParamId {
		ENUMS(FADER_PARAM, 16),
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(CV_OUTPUT, 16),
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(FADER_LIGHTS, 16),
		LIGHTS_LEN
	};

	midi::InputQueue midiInput;

	int8_t faderCCs[16];
	int8_t ccValues[128];

	_16n() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		for (int i=0; i < 16; i++) {
			configParam(FADER_PARAM + i, 0.f, 1.f, 0.f, string::f("Fader %d", i+1), " v", 0, 10);
			configOutput(CV_OUTPUT + i, string::f("CV %d", i+1));

			lights[FADER_LIGHTS + i].setBrightness(1);
			
			// TODO: make this configurable? maybe?
			// default 16n CC values are 32-47 
			faderCCs[i] = 32+i;
		}
		onReset();
	}

	void onReset() override { 
		midiInput.reset();
	}

	void process(const ProcessArgs& args) override {

		midi::Message msg;
		while (midiInput.tryPop(&msg, args.frame)) {
			processMessage(msg);
		}

		if (midiInput.getDeviceId() >= 0) { 
			// Midi device connected
			for (int id = 0; id < 16; id++) {				
				int8_t cc = faderCCs[id];				
				int16_t cellValue = int16_t(ccValues[cc]) * 128;
				float value = float(cellValue) / (128 * 127);
				value = clamp(value, 0.f, 1.f);
				params[FADER_PARAM + id].setValue(value);
				outputs[CV_OUTPUT + id].setVoltage(value * 10.f);			
			}
		}
		else {
			// use the slider manual control if no 16n faderbank is connected
			for (int id = 0; id < 16; id++) {	
				outputs[CV_OUTPUT + id].setVoltage(params[FADER_PARAM + id].getValue() * 10.f);				
			}
		}
	}

	void processMessage(const midi::Message& msg) {		
		switch (msg.getStatus()) {
			// cc value
			case 0xb: {
				processCC(msg);
			} break;
			default: break;
		}
	}

	void processCC(const midi::Message& msg) {
		uint8_t cc = msg.getNote();
		if (msg.bytes.size() < 2)
			return;
		// Allow CC to be negative if the 8th bit is set.
		// The gamepad driver abuses this, for example.
		// Note: I don't think I need this but left it in anyway 
		// Cast uint8_t to int8_t
		int8_t value = msg.bytes[2];
		ccValues[cc] = value;
	}
};


struct _16nWidget : ModuleWidget {
	_16nWidget(_16n* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/16n.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createLightParamCentered<VCVLightSliderTall<YellowLight>>(mm2px(Vec(9.18, 75.001)), module, _16n::FADER_PARAM + 0, _16n::FADER_LIGHTS + 0));
		addParam(createLightParamCentered<VCVLightSliderTall<YellowLight>>(mm2px(Vec(22.18, 75.001)), module, _16n::FADER_PARAM + 1, _16n::FADER_LIGHTS + 1));
		addParam(createLightParamCentered<VCVLightSliderTall<YellowLight>>(mm2px(Vec(35.18, 75.001)), module, _16n::FADER_PARAM + 2, _16n::FADER_LIGHTS + 2));
		addParam(createLightParamCentered<VCVLightSliderTall<YellowLight>>(mm2px(Vec(48.18, 75.001)), module, _16n::FADER_PARAM + 3, _16n::FADER_LIGHTS + 3));
		addParam(createLightParamCentered<VCVLightSliderTall<BlueLight>>(mm2px(Vec(61.18, 75.001)), module, _16n::FADER_PARAM + 4, _16n::FADER_LIGHTS + 4));
		addParam(createLightParamCentered<VCVLightSliderTall<BlueLight>>(mm2px(Vec(74.18, 75.001)), module, _16n::FADER_PARAM + 5, _16n::FADER_LIGHTS + 5));
		addParam(createLightParamCentered<VCVLightSliderTall<BlueLight>>(mm2px(Vec(87.18, 75.001)), module, _16n::FADER_PARAM + 6, _16n::FADER_LIGHTS + 6));
		addParam(createLightParamCentered<VCVLightSliderTall<BlueLight>>(mm2px(Vec(100.18, 75.001)), module, _16n::FADER_PARAM + 7, _16n::FADER_LIGHTS + 7));
		addParam(createLightParamCentered<VCVLightSliderTall<GreenLight>>(mm2px(Vec(113.18, 75.001)), module, _16n::FADER_PARAM + 8, _16n::FADER_LIGHTS + 8));
		addParam(createLightParamCentered<VCVLightSliderTall<GreenLight>>(mm2px(Vec(126.18, 75.001)), module, _16n::FADER_PARAM + 9, _16n::FADER_LIGHTS + 9));
		addParam(createLightParamCentered<VCVLightSliderTall<GreenLight>>(mm2px(Vec(139.18, 75.001)), module, _16n::FADER_PARAM + 10, _16n::FADER_LIGHTS + 10));
		addParam(createLightParamCentered<VCVLightSliderTall<GreenLight>>(mm2px(Vec(152.18, 75.001)), module, _16n::FADER_PARAM + 11, _16n::FADER_LIGHTS + 11));
		addParam(createLightParamCentered<VCVLightSliderTall<WhiteLight>>(mm2px(Vec(165.18, 75.001)), module, _16n::FADER_PARAM + 12, _16n::FADER_LIGHTS + 12));
		addParam(createLightParamCentered<VCVLightSliderTall<WhiteLight>>(mm2px(Vec(178.18, 75.001)), module, _16n::FADER_PARAM + 13, _16n::FADER_LIGHTS + 13));
		addParam(createLightParamCentered<VCVLightSliderTall<WhiteLight>>(mm2px(Vec(191.18, 75.001)), module, _16n::FADER_PARAM + 14, _16n::FADER_LIGHTS + 14));
		addParam(createLightParamCentered<VCVLightSliderTall<WhiteLight>>(mm2px(Vec(204.18, 75.001)), module, _16n::FADER_PARAM + 15, _16n::FADER_LIGHTS + 15));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.18, 109.355)), module, _16n::CV_OUTPUT + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.18, 109.355)), module, _16n::CV_OUTPUT + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.18, 109.355)), module, _16n::CV_OUTPUT + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.18, 109.355)), module, _16n::CV_OUTPUT + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(61.18, 109.355)), module, _16n::CV_OUTPUT + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.18, 109.355)), module, _16n::CV_OUTPUT + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(87.18, 109.355)), module, _16n::CV_OUTPUT + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(100.18, 109.355)), module, _16n::CV_OUTPUT + 7));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.18, 109.355)), module, _16n::CV_OUTPUT + 8));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(126.18, 109.355)), module, _16n::CV_OUTPUT + 9));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(139.18, 109.355)), module, _16n::CV_OUTPUT + 10));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(152.18, 109.355)), module, _16n::CV_OUTPUT + 11));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(165.18, 109.355)), module, _16n::CV_OUTPUT + 12));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(178.18, 109.355)), module, _16n::CV_OUTPUT + 13));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(191.18, 109.355)), module, _16n::CV_OUTPUT + 14));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(204.18, 109.355)), module, _16n::CV_OUTPUT + 15));

		// Todo? replace this with hard-coded? or not?
		MidiDisplay* display = createWidget<MidiDisplay>(mm2px(Vec(86.360, 13.048)));
		display->box.size = mm2px(Vec(40.64, 29.012));
		display->setMidiPort(module ? &module->midiInput : NULL);
		addChild(display);
	}
}; 

Model* model_16n = createModel<_16n, _16nWidget>("16n");