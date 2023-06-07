#include "gm_tank.h"
#include "gm_bullet.h"

MdlTank::MdlTank() {
	lostSe = LoadSoundMem("sound/bomb.mp3");
}
MdlTank::MdlTank(tnl::Vector3 pos, const char* texture) {
	lostSe = LoadSoundMem("sound/bomb.mp3");
	pos_ = pos;
	this->texture_ = texture;

}
MdlTank::~MdlTank() {

}
MdlTank* MdlTank::Create(tnl::Vector3 pos, const char* texture) {
	MdlTank* mdl = new MdlTank(pos, texture);
	mdl->parts_.resize(e_parts_max);

	//ŽÔ‘Ì
	Parts* body = new Parts();
	body->mesh_ = dxe::Mesh::CreateBox(24);
	body->mesh_->setTexture(dxe::Texture::CreateFromFile(texture));
	body->ofs_rot_ = tnl::Quaternion::RotationAxis({ 1,0,0 }, tnl::ToRadian(90));
	mdl->parts_[e_body] = body;

	//–C“ƒ
	Parts* turret = new Parts();
	turret->mesh_ = dxe::Mesh::CreateCylinder(8, 12);
	turret->mesh_->setTexture(dxe::Texture::CreateFromFile(texture));
	turret->ofs_pos_ = { 0,18,0 };
	mdl->parts_[e_turret] = turret;

	//–Cg
	Parts* gunbarrel = new Parts();
	gunbarrel->mesh_ = dxe::Mesh::CreateCylinder(4, 30);
	gunbarrel->mesh_->setTexture(dxe::Texture::CreateFromFile(texture));
	gunbarrel->ofs_pos_ = { 0,20,12 };
	gunbarrel->ofs_rot_ = tnl::Quaternion::RotationAxis({ 1, 0, 0 }, tnl::ToRadian(80));
	mdl->parts_[e_gunbarrel] = gunbarrel;

	mdl->update(0);

	return mdl;
}

void MdlTank::bomberdment() {
	if (bullets.size() < 5 && isAlive) {
		auto bullet = MdlBullet::Create(pos_ + tnl::Vector3{ 0,20,0 }, dir_z, texture_);
		bullets.emplace_back(bullet);
	}
}

void MdlTank::update(float delta_time) {
	prev_pos = pos_;
	dir_z = tnl::Vector3::TransformCoord(dir_z_init, rot_);
	Model::update(delta_time);
}

void MdlTank::move(dxe::Camera* camera, int index, tnl::Vector3 target) {
	if (isAlive) {
		tnl::Vector3 dir[4] = {
			camera->front().xz(),
			camera->right().xz(),
			camera->back().xz(),
			camera->left().xz(),
		};
		tnl::Vector3 move_v = { 0,0,0 };
		move_v += dir[index];
		move_v.normalize();
		this->rot_ = tnl::Quaternion::LookAtAxisY(this->pos_, target);
		this->pos_ += move_v * 2.0f;
	}
}

void MdlTank::lost() {
	if (isAlive) {
		PlaySoundMem(lostSe, DX_PLAYTYPE_BACK, 1);
		isAlive = false;
	}
}