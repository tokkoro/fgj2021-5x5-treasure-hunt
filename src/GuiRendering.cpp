#include "GuiRendering.h"

#include "GuiRenderingState.h"
#include <vector>
#include <assert.h>
#include <mutex>
#include <atomic>

struct State
{
	State()
	{
		clipRectStack.push_back(getAntiClipRect());
	}

	GuiRenderingState state;
	std::vector<ClipRect> clipRectStack;
	std::mutex m;
};

static State &getState()
{
	static State *state = new State();
	return *state;
}

static int getThreadNumber()
{
	static std::atomic<int> ctr(10);
	thread_local int threadNumber = ctr.fetch_add(1);
	return threadNumber;
}

static std::atomic<int> &getActiveThreadNumberMutable()
{
	static std::atomic<int> ctr(-1);
	return ctr;
}

static int getActiveThreadNumber()
{
	return getActiveThreadNumberMutable();
}

void GuiRendering::render(const GuiRenderInfo &guiRenderInfo)
{
	getState().state.pushGuiRenderInfo(guiRenderInfo);
}

static bool isActiveThread()
{
	return getActiveThreadNumber() == getThreadNumber();
}

static float max(float a, float b)
{
	return a >= b ? a : b;
}

static float min(float a, float b)
{
	return a <= b ? a : b;
}

void GuiRendering::pushClipRect(float x, float y, float w, float h)
{
	assert(isActiveThread());

	State &state = getState();
	ClipRect prev = state.clipRectStack.back();
	ClipRect next(x, y, w, h);
	next.x = max(x, prev.x);
	next.y = max(y, prev.y);
	next.w = min(x + w, prev.x + prev.w) - next.x;
	next.h = min(y + h, prev.y + prev.h) - next.y;
	state.clipRectStack.push_back(next);
	state.state.setClipRect(next);
}

void GuiRendering::pushAntiClipRect()
{
	assert(isActiveThread());

	State &state = getState();
	ClipRect next = getAntiClipRect();
	state.clipRectStack.emplace_back(next);
	state.state.setClipRect(next);
}

void GuiRendering::popClipRect()
{
	assert(isActiveThread());

	State &state = getState();
	state.clipRectStack.pop_back();
	assert(state.clipRectStack.size() > 0 && "Popped too many clip rects.");
	state.state.setClipRect(state.clipRectStack.back());
}

void GuiRendering::startThread()
{
	State &state = getState();
	state.m.lock();
	assert(getActiveThreadNumber() == -1);
	getActiveThreadNumberMutable() = getThreadNumber();
}

void GuiRendering::endThread()
{
	assert(getActiveThreadNumber() == getThreadNumber());
	getActiveThreadNumberMutable() = -1;
	State &state = getState();
	state.m.unlock();
}

void GuiRendering::flush(GuiRenderingState &result)
{
	assert(getActiveThreadNumber() == getThreadNumber());
	State &state = getState();
	state.state.renderInfos.swap(result.renderInfos);
	state.state.renderInfos.clear();
}