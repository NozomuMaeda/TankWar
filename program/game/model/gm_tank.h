#pragma once
#include "gm_model.h"
#include "gm_bullet.h"

class GmCamera;
class MdlBullet;
class MdlTank :public Model {
public:
	enum
	{
		e_body,
		e_turret,
		e_gunbarrel,
		e_parts_max
	};
	tnl::Vector3 prev_pos;
	const char* texture_ = nullptr;
	float gunbarrel_angle = 0;
	MdlTank();
	MdlTank(tnl::Vector3, const char*);
	~MdlTank();
	int eDir = 2;
	int lostSe = 0;
	tnl::Vector3 size = { 24,40,24 };//������p�T�C�Y
	tnl::Vector3 dir_z_init = { 0,0,1 };//��{�̌����Ă����
	tnl::Vector3 dir_z = { 0,0,1 };//�������Ƃ�
	std::list<MdlBullet*> bullets;//�����̖C�e
	void update(float) override;
	void move(dxe::Camera*, int, tnl::Vector3);
	void lost();
	void bomberdment();
	static MdlTank* Create(tnl::Vector3, const char*);
};