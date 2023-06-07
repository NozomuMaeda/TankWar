#include "../gm_manager.h"
#include "gm_scene_title.h"
#include "gm_scene_play.h"


void SceneTitle::initialzie() {
}

void SceneTitle::update(float delta_time)
{
	GameManager* mgr = GameManager::GetInstance();

	if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::IN_LEFT)) {
		mgr->chengeScene(new ScenePlay());
	}
}

void SceneTitle::render()
{
	SetFontSize(100);
	DrawStringEx(DXE_WINDOW_WIDTH / 2.0 - 300, DXE_WINDOW_HEIGHT / 2.0 - 150, 0xff009911, "�^���N�E�H�[");
	SetFontSize(20);
	DrawStringEx(DXE_WINDOW_WIDTH / 2.0 - 300, DXE_WINDOW_HEIGHT / 2.0 + 100, -1, "AWSD�ňړ��A�}�E�X�J�[�\���ŏƏ������킹���N���b�N�ŖC��");
	DrawStringEx(DXE_WINDOW_WIDTH / 2.0 - 180, DXE_WINDOW_HEIGHT / 2.0 + 200, -1, "�������ł����獶�N���b�N�ŏo���I");
}