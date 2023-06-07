#include "../gm_manager.h"
#include "../gm_camera.h"
#include "../model/gm_anim_sprite3d.h"
#include "../model/gm_airplane.h"
#include "../model/gm_tank.h"
#include "../model/gm_wall.h"
#include "gm_scene_play.h"
#include "gm_scene_result.h"

tnl::Quaternion	fix_rot;

ScenePlay::~ScenePlay() {
	delete camera_;
	for (auto box : wall_boxs_) delete box;
}

void ScenePlay::initialzie() {
	SetFontSize(20);
	srand(time(0));
	//戦車
	tank = MdlTank::Create({ 0,12,0 }, "graphics/green.bmp");
	enemy_tanks.emplace_back(MdlTank::Create({ 400,12,400 }, "graphics/purple.bmp"));
	enemy_tanks.emplace_back(MdlTank::Create({ -400,12,-400 }, "graphics/purple.bmp"));
	enemy_tanks.emplace_back(MdlTank::Create({ -400,12,400 }, "graphics/purple.bmp"));
	enemy_tanks.emplace_back(MdlTank::Create({ 400,12,-400 }, "graphics/purple.bmp"));

	//地面
	ground = dxe::Mesh::CreatePlane({ 1000,1000,0 });
	ground->setTexture(dxe::Texture::CreateFromFile("graphics/512.bmp"));
	ground->rot_q_ = tnl::Quaternion::RotationAxis({ 1,0,0 }, tnl::ToRadian(90));

	//------------------------------------
	// 授業配布サンプルの転用
	// 使用するテクスチャを事前ロード
	Shared<dxe::Texture> tex;
	tex = dxe::Texture::CreateFromFile("graphics/512.bmp");

	// ボックスをあらかじめ作成
	dxe::Mesh* origin_boxs;
	origin_boxs = dxe::Mesh::CreateBoxMV(WALL_SIZE);
	origin_boxs->setTexture(tex);

	// csv からフィールド情報を読み込んであらかじめ作成してある
	// ボックスをクローンして生成することで生成速度アップ
	std::vector<std::vector<std::string>> csv = tnl::LoadCsv("map.csv");
	for (int i = 0; i < csv.size(); ++i) {
		for (int k = 0; k < csv[i].size(); ++k) {
			int n = std::atoi(csv[i][k].c_str());
			// 0 は地面扱いとして飛ばす
			if (0 == n) continue;
			dxe::Mesh* mesh = origin_boxs->createClone();
			mesh->pos_ = { SX + k * WALL_SIZE, 25, SZ - i * WALL_SIZE };
			wall_boxs_.emplace_back(mesh);
		}
	}

	//カーソル
	target = dxe::Mesh::CreateDome(1);
	target->setTexture(dxe::Texture::CreateFromFile("graphics/red1.bmp"));

	camera_ = new GmCamera();
	camera_->target_distance_ = 500;
}

//更新
void ScenePlay::update(float delta_time)
{
	//ゲームマネージャーのインスタンスを生成/取得
	GameManager* mgr = GameManager::GetInstance();
	camera_->target_ = tank->pos_;

	//------------------------------------------------------------------
	//
	// 移動制御
	//
	if (tnl::Input::IsKeyDown(eKeys::KB_W, eKeys::KB_D, eKeys::KB_S, eKeys::KB_A)) {
		/*move_v.normalize();
		tank->rot_.slerp(tnl::Quaternion::LookAtAxisY(tank->pos_, tank->pos_ + move_v), 0.3f);
		tank->pos_ += move_v * 2.0f;*/
		tnl::Input::RunIndexKeyDown([&](uint32_t idx) {
			tank->move(camera_, idx, target->pos_);
			/*move_v += dir[idx];*/
			}, eKeys::KB_W, eKeys::KB_D, eKeys::KB_S, eKeys::KB_A);
	}

	//------------------------------------------------------------------
	//
	// カメラ制御
	//
	//ズームイン・アウト
	if (tnl::Input::IsKeyDown(eKeys::KB_Z)) {
		camera_->target_distance_ += 1.0f;
	}
	if (tnl::Input::IsKeyDown(eKeys::KB_X)) {
		camera_->target_distance_ -= 1.0f;
	}

	//砲撃
	if (tnl::Input::IsMouseTrigger(tnl::Input::eMouseTrigger::IN_LEFT)) {
		tank->bomberdment();
	}

	tank->update(delta_time);

	// ---------------------------------
	// 授業配布サンプルの転用
	// 衝突判定と補正
	tnl::Vector3 boxSize = { WALL_SIZE, WALL_SIZE, WALL_SIZE };
	//戦車対壁
	if (tank->isAlive) {
		// 操作機体に近い順にソート
		wall_boxs_.sort([&](const dxe::Mesh* l, const dxe::Mesh* r) {
			float ld = (tank->pos_ - l->pos_).length();
			float rd = (tank->pos_ - r->pos_).length();
			return ld < rd;
			});
		for (auto box : wall_boxs_) {
			if (tnl::IsIntersectAABB(tank->pos_, tank->size, box->pos_, boxSize)) {
				int n = tnl::GetCorrectPositionIntersectAABB(tank->prev_pos, tank->size, box->pos_, boxSize, tank->pos_);
			}
		}
		for (auto etank : enemy_tanks) {
			//戦車対戦車
			if (tnl::IsIntersectAABB(tank->pos_, tank->size, etank->pos_, etank->size)) {
				int n = tnl::GetCorrectPositionIntersectAABB(tank->prev_pos, tank->size, etank->pos_, etank->size, tank->pos_);
			}
		}
	}
	//自機砲弾
	// 砲弾に近い順にソート
	for (auto bullet : tank->bullets) {
		bullet->update(delta_time);
		bullet->move();
		wall_boxs_.sort([&](const dxe::Mesh* l, const dxe::Mesh* r) {
			float ld = (bullet->pos_ - l->pos_).length();
			float rd = (bullet->pos_ - r->pos_).length();
			return ld < rd;
			});
		for (auto box : wall_boxs_) {
			if (tnl::IsIntersectAABB(bullet->pos_, bullet->size, box->pos_, boxSize)) {
				bullet->isAlive = false;
			}
		}
		//砲弾対戦車
		for (auto etank : enemy_tanks) {

			if (tnl::IsIntersectAABB(bullet->pos_, bullet->size, etank->pos_, etank->size)) {
				bullet->isAlive = false;
				etank->lost();
			}
		}
	}

	//敵戦車対壁
	for (auto etank : enemy_tanks) {
		etank->update(delta_time);
		if (rand() % 60 == 0)etank->bomberdment();
		// 砲弾に近い順にソート
		for (auto bullet : etank->bullets) {
			bullet->update(delta_time);
			bullet->move();
			wall_boxs_.sort([&](const dxe::Mesh* l, const dxe::Mesh* r) {
				float ld = (bullet->pos_ - l->pos_).length();
				float rd = (bullet->pos_ - r->pos_).length();
				return ld < rd;
				});
			for (auto box : wall_boxs_) {
				if (tnl::IsIntersectAABB(bullet->pos_, bullet->size, box->pos_, boxSize)) {
					bullet->isAlive = false;
				}
			}
			//砲弾対自機戦車
			if (tnl::IsIntersectAABB(bullet->pos_, bullet->size, tank->pos_, tank->size)) {
				mgr->result = mgr->lose;//負け
				bullet->isAlive = false;
				tank->lost();
				mgr->chengeScene(new SceneResult());
			}
		}
		etank->move(camera_, etank->eDir, tank->pos_);
		wall_boxs_.sort([&](const dxe::Mesh* l, const dxe::Mesh* r) {
			float ld = (etank->pos_ - l->pos_).length();
			float rd = (etank->pos_ - r->pos_).length();
			return ld < rd;
			});
		for (auto box : wall_boxs_) {
			if (tnl::IsIntersectAABB(etank->pos_, etank->size, box->pos_, boxSize)) {
				int n = tnl::GetCorrectPositionIntersectAABB(etank->prev_pos, etank->size, box->pos_, boxSize, etank->pos_);
				etank->eDir += 1 + rand() % 3;
				etank->eDir = etank->eDir % 4;
			}
		}
	}
	//シーンをリザルトに切り替える
	if (enemy_tanks.size() < 1) {
		mgr->result = mgr->win;//勝ち
		mgr->chengeScene(new SceneResult());
	}
}

//描画関連
void ScenePlay::render()
{
	camera_->update();

	auto bul = tank->bullets.begin();
	while (bul != tank->bullets.end()) {
		if (!(*bul)->isAlive) {
			delete (*bul);
			bul = tank->bullets.erase(bul);
			continue;
		}
		(*bul)->render(camera_);
		bul++;
	}
	//敵戦車レンダーと死亡時の除外
	auto eTan = enemy_tanks.begin();
	while (eTan != enemy_tanks.end()) {
		if (!(*eTan)->isAlive) {
			delete (*eTan);
			eTan = enemy_tanks.erase(eTan);
			continue;
		}
		(*eTan)->render(camera_);
		auto eBul = (*eTan)->bullets.begin();
		while (eBul != (*eTan)->bullets.end()) {
			if (!(*eBul)->isAlive) {
				delete (*eBul);
				eBul = (*eTan)->bullets.erase(eBul);
				continue;
			}
			(*eBul)->render(camera_);
			eBul++;
		}
		eTan++;
	}

	ground->render(camera_);
	for (auto box : wall_boxs_) box->render(camera_);
	for (auto etank : enemy_tanks)	etank->render(camera_);
	tank->render(camera_);
	target->render(camera_);

	//DrawOBB(camera_, tank->pos_, tank->rot_, { 32, 48, 32 });//キャラを囲む直方体枠

	//マウスポインターからレイを飛ばし地表との交点を探す
	tnl::Vector3 msv = tnl::Input::GetMousePosition();
	tnl::Vector3 ray = tnl::Vector3::CreateScreenRay(
		msv.x
		, msv.y
		, camera_->screen_w_
		, camera_->screen_h_
		, camera_->view_
		, camera_->proj_);
	tnl::Vector3 hit;

	if (tnl::IsIntersectLinePlane(camera_->pos_, camera_->pos_ + (ray * 10000.0f), { 10, 20, 10 }, { 0, 1, 0 }, &hit)) {
		target->pos_ = hit;
	}

	//格子のやつ
	DrawGridGround(camera_, 50, 20);

	DrawStringEx(50, 50, -1, "移動 [ ← : A ] [ ↑ : W ] [ → : D ] [ ↓ : S ]");
	DrawStringEx(50, 70, -1, "カーソルで目標を定め、左クリックで砲撃");
	DrawStringEx(850, 50, -1, "残り敵戦車 %d", enemy_tanks.size());
}
