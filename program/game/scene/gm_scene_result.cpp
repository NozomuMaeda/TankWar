#include "../gm_manager.h"
#include "gm_scene_result.h"
#include "gm_scene_title.h"

void SceneResult::initialzie() {
}

void SceneResult::update(float delta_time)
{
	GameManager* mgr = GameManager::GetInstance();

	SetFontSize(100);
	if (mgr->result == mgr->win) {
		DrawStringEx(DXE_WINDOW_WIDTH / 2.0 - 100, DXE_WINDOW_WIDTH / 2.0 - 175, 0xffff0000, "Ÿ—˜");
	}
	else if (mgr->result == mgr->lose) {
		DrawStringEx(DXE_WINDOW_WIDTH / 2.0 - 100, DXE_WINDOW_WIDTH / 2.0 - 175, 0xff0000ff, "”s–k");
	}

	if (tnl::Input::IsKeyDownTrigger(eKeys::KB_RETURN)) {
		mgr->chengeScene(new SceneTitle());
	}
}

void SceneResult::render()
{

}
