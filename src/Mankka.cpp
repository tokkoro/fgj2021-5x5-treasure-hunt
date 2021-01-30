#include "Mankka.h"

#include <assert.h>
#include <random>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include "Resources.h"

namespace
{

struct SoundPreset
{
	std::vector<SoundResourceName> pool;
};

struct PlaybackState
{
	std::unordered_map<SoundResourceName, sf::Sound> sounds;
};

struct MusicState
{
	MusicEnvName env = MusicEnvName::silence;
	MusicResourceName resourceName;
	std::unique_ptr<sf::Music> playingMusic;
};

}

struct Mankka::MankkaState
{
	std::unordered_map<SoundPresetName, SoundPreset> presets;
	std::unordered_map<SoundPresetName, PlaybackState> playbackStates;
	std::unordered_map<MusicEnvName, MusicResourceName> musicEnvs;
	MusicState musicState;
	std::default_random_engine rng;
};

Mankka::Mankka()
{
	auto &resources = Resources::getResources();

	state.reset(new MankkaState());

	state->presets[SoundPresetName::bonk] =
	{
		{ SoundResourceName::bonk }
	};

	state->presets[SoundPresetName::hajotus] =
	{
		{ SoundResourceName::break_ }
	};

	state->presets[SoundPresetName::button_press] =
	{
		{ SoundResourceName::button_press }
	};

	state->presets[SoundPresetName::gameover] =
	{
		{ SoundResourceName::gameover }
	};

	state->presets[SoundPresetName::monster_move] =
	{
		{
			SoundResourceName::miu,
			SoundResourceName::mjau
		}
	};

	state->presets[SoundPresetName::monster_ded] =
	{
		{ SoundResourceName::monster_ded }
	};

	state->presets[SoundPresetName::mouse_hit_wall] =
	{
		{ SoundResourceName::mouse_hit_wall }
	};

	state->presets[SoundPresetName::mouse_eat] =
	{
		{ SoundResourceName::mouse_mus_mus }
	};

	state->presets[SoundPresetName::movetiles] =
	{
		{
			SoundResourceName::movetiles_1,
			SoundResourceName::movetiles_2,
			SoundResourceName::movetiles_3,
		}
	};

	state->presets[SoundPresetName::mouse_dead] =
	{
		{ SoundResourceName::o_oui }
	};

	state->presets[SoundPresetName::peepeep] =
	{
		{ SoundResourceName::peepeep }
	};


	state->presets[SoundPresetName::togglebutton] =
	{
		{
			SoundResourceName::togglebutton_1,
			SoundResourceName::togglebutton_2
		}
	};

	for (const auto &presetsIt : state->presets)
	{
		for (SoundResourceName resName : presetsIt.second.pool)
		{
			sf::SoundBuffer &buf = resources.soundEffects[resName];
			state->playbackStates[presetsIt.first].sounds[resName] = sf::Sound(buf);
		}

		assert(presetsIt.second.pool.size() > 0);
	}

	// Music envs

	state->musicEnvs = {
		{ MusicEnvName::ingame, MusicResourceName::thebiisi }
	};
}

Mankka &Mankka::getMankka()
{
	static Mankka mankka;
	return mankka;
}

void Mankka::play(SoundPresetName presetId)
{
	const auto &pool = state->presets[presetId].pool;
	PlaybackState &pbs = state->playbackStates[presetId];

	std::uniform_int_distribution<size_t> distribution(0, pool.size() - 1);
	size_t pickIndex = distribution(state->rng);

	SoundResourceName effectName = pool[pickIndex];

	pbs.sounds[effectName].play();
}

void Mankka::play(MusicEnvName envName)
{
	if (state->musicState.env == envName)
		return;

	state->musicState.env = envName;

	if (state->musicState.env == MusicEnvName::silence)
	{
		state->musicState.playingMusic.reset();
	}
	else
	{
		state->musicState.resourceName = state->musicEnvs[envName];

		state->musicState.playingMusic =
			Resources::getResources().getMusic(state->musicState.resourceName);

		state->musicState.playingMusic->setLoop(true);
		state->musicState.playingMusic->play();
	}
}

void Mankka::test()
{
	play(SoundPresetName::movetiles);
}
