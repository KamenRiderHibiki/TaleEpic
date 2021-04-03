#pragma once
#ifndef INGAMEOBJECT_H
#define INGAMEOBJECT_H

#include "DirectX.h"

#define BAS_ATTR_DIV_CON 33	//basic attribute divide constant
#define BAS_CON_ATTR_CON 3	//basic consumable attribute divide constant
#define EMY_SPN_DIS 500		//enemy spawn distance
#define EMY_SPN_NUM 10		//maxium number of wild emeny spawn
#define EMY_SPN_CD 5.0		//enemy spawn cooldown
#define DMG_SHOW_TIME 1000	//damage show time
//属性
enum Type
{
	//C++的enum推测赋值特性
	None=-1,
	Grass=0,
	Water=1,
	Fire=2,
	Ground,
	Wind,
	Thunder,
	Light,
	Shadow=7
};

//天气
enum Weather {
	NoWeather=0,
	RainWeather,
	WindWeather,
	SnowWeather,
	SandWeather
};

//属性克制
float TypeRelation[8][8]=
{
	{0.5f,2.0f,0.5f,2.0f,0.5f,0.5f,1.0f,1.0f},
	{0.5f,0.5f,2.0f,2.0f,1.0f,0.5f,1.0f,1.0f},
	{2.0f,0.5f,0.5f,0.5f,2.0f,1.0f,1.0f,1.0f},
	{0.5f,0.5f,2.0f,0.5f,2.0f,2.0f,1.0f,1.0f},
	{2.0f,1.0f,1.0f,2.0f,0.5f,1.0f,1.0f,1.0f},
	{0.5f,2.0f,1.0f,0.5f,1.0f,0.5f,1.0f,1.0f},
	{1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0.5f,2.0f},
	{1.0f,1.0f,0.5f,1.0f,1.0f,1.0f,2.0f,1.0f}
};

//稀有度
enum RareClass {
	Common,
	Usual,
	Normal,
	Ordinary,
	Rare,
	Scarce,
	Incredible,
	Fabulous
};
//游戏状态
enum GameState {
	Init,
	play_ing,
	play_dialog,
	play_pause,
	menu,
	option,
	end,
};

//是否包含部分
bool RECTContain(RECT& rect1, RECT& rect2)
{
	//如果r1在左，那r1的右边界必须小于r2左边界否则在x轴方向上二者相交
	bool x = rect1.left < rect2.left ? rect1.right < rect2.left ? false : true : rect2.right < rect1.left ? false : true;
	//y轴也做类似处理
	bool y = rect1.top < rect2.top ? rect1.bottom < rect2.top ? false : true : rect2.bottom < rect1.top ? false : true;
	//都交者相交
	return x&&y;
}
//是否完全包含
inline bool RECTInside(RECT& in, RECT& out)
{
	//检查in的左右上下边界是否都在out里边（in的left和top>out，right和bottom小于out）
	return in.left > out.left&&in.right < out.right&&in.top > out.top&&in.bottom < out.bottom;
}
//是否包含点
inline bool POINTInside(int x, int y, RECT& out)
{
	//检查X,Y是否都在out里边（in的left和top>out，right和bottom小于out）
	return x > out.left&&x < out.right&&y > out.top&&y < out.bottom;
}

//种族
struct SPECIES final//C++的final关键字用于禁止从类继承/阻止一个虚函数的重载
{
	int ID;
	int protoID;
	int Strength, Stamina, MagicPower, MagicVolume, Intelligence, Agile, LifePower;
	string Name;
	string protoName;
	/*
	SPECIES()
	{
	ID = protoID = Strength = Stamina = MagicPower = MagicVolume = Intelligence = Agile = LifePower = 0;
	Name = protoName = "";
	}
	*/
	SPECIES(int id = 0, int atk = 0, int stma = 0, int matk = 0, int mana = 0, int iq = 0, int agile = 0, int hp = 0, string name = "", int protoI = 0, string protoN = "")
	{
		ID = id, protoID = protoI, Strength = atk, Stamina = stma, MagicPower = matk, MagicVolume = mana, Intelligence = iq, Agile = agile, LifePower = hp;
		Name = name, protoName = protoN;
	}
};

//国家
struct Country
{
	int ID;
	string name;
};

//攻击技能
struct ATTACKSKILL
{
	LPDIRECT3DTEXTURE9* skillTex;
	float attackCD;
	int attackDamage, criticalRate;
	ATTACKSKILL()
	{
		skillTex = nullptr;
		attackCD = 1.0f;
		attackDamage = 0, criticalRate = 0;
	}
	ATTACKSKILL(LPDIRECT3DTEXTURE9* tex, float cd, int damage, int cr)
	{
		skillTex = tex;
		attackCD = cd;
		attackDamage = damage, criticalRate = cr;
	}
};
//精灵纹理信息
struct SPRITEINF
{
	int Columns = 1;
	int Startframe = 0;
	int Endframe = 0;
	int Delay = 100;
	int DirectionType = 0;
	float Scaling = 1.0f;
	float DrawoffsetY = 0.85f;
	LPDIRECT3DTEXTURE9* spriteTex;
	LPDIRECT3DTEXTURE9 spriteIcon;
	LPDIRECT3DTEXTURE9 spriteDraw;
	string name;
	SPRITEINF()
	{
		//*spriteTex = spriteIcon = spriteDraw = nullptr;
	}
	SPRITEINF(LPDIRECT3DTEXTURE9& sp, int col = 1, float scale = 1.0f, int startframe = 0, int endframe = 0, int delay = 66, int type = 1, float drawoffsetY = 0.85f, LPDIRECT3DTEXTURE9 icon=nullptr, LPDIRECT3DTEXTURE9 draw=nullptr)
	{
		spriteTex = &sp;
		Columns = col, Scaling = scale, Startframe = startframe, Endframe = endframe, DrawoffsetY = drawoffsetY;
		Delay = delay, DirectionType = type;
		spriteIcon = icon, spriteDraw = draw;
	}
};
//角色
struct CHARACTOR :SPRITE
{
	//位置
	float position[3] = { 0.0f,0.0f,0.0f };
	float center[2] = { 0,0 };
	float moveSpeed = 200.0f;//移动速度
	float animateTick = 0.0f;//计算帧间经过时间
	float attackCD = 0.0f;
	float MPrecoverCD = 0.0f;
	int target[3] = { -1,-1,-1 };//移动目标
	int row = 1;//精灵行图片数量
	int animeIndex[8][3];//动画索引
	int texSize[2] = { 0,0 };
	int faceDirection = 0;
	int level = 1;
	bool isMannualMoving = false;
	bool constant = false;
	//Health Point,Mana,Attack,Defence,Magic Attatk,Magic Defence,Stamina
	int HP, MaxHP, MP, MaxMP, ATK, DEF, MAK, MDF, SMA;
	//SPRITE spriteData;
	LPDIRECT3DTEXTURE9* sprite;
	LPDIRECT3DTEXTURE9 icon;
	LPDIRECT3DTEXTURE9 draw;
	string tag = "null";
	string race = "null";
	string profession = "null";
	string lastTerrian = "null";
	string currTerrian = "null";
	CHARACTOR(const SPRITE sprite) :SPRITE()
	{
		x = sprite.x, y = sprite.y;
		frame = sprite.frame, columns = sprite.columns;
		width = sprite.width, height = sprite.height;
		scaling = sprite.scaling, rotation = sprite.rotation;
		startframe = sprite.startframe, endframe = sprite.endframe;
		direction = sprite.direction;
		starttime = sprite.starttime, delay = sprite.delay;
		velx = sprite.velx, vely = sprite.vely;
		color = sprite.color;
		row = (endframe + 1) / columns;
		BuildAnimeIndex(columns, row);
		center[0] = float(width*0.5), center[1] = float(height *0.85);//很多精灵图像底部离纹理块底部大约是全高度的15%，因此上移50%-15%即到达所需位置
		texSize[0] = width * columns, texSize[1] = height * row;
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				animeIndex[i][j] = -1;
			}
		}
		position[0] = position[1] = position[2] = 0.0f;
		MP = MaxMP = ATK = DEF = MAK = MDF = SMA = 0;
		HP = MaxHP = 1;
		row = level = 1;
		faceDirection = 0;
		target[0] = target[1] = target[2] = -1;
		this->sprite = NULL;
	}
	CHARACTOR(int Width = 0, int Height = 0, int Frame = 0, int Columns = 1, float Scaling = 1.0f, float Rotation = 0.0f, int Startframe = 0, int Endframe = 0) :SPRITE()
	{
		width = Width, height = Height;
		frame = Frame, columns = Columns;
		scaling = Scaling, rotation = Rotation;
		startframe = Startframe, endframe = Endframe;
		row = (endframe + 1) / columns;
		texSize[0] = width * columns, texSize[1] = height * row;
		color = D3DCOLOR_XRGB(255, 255, 255);
		center[0] = float(width*0.5), center[1] = float(height *0.85);//很多精灵图像底部离纹理块底部大约是全高度的15%，因此上移50%-15%即到达所需位置
		this->sprite = NULL;
		faceDirection = 0;
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				animeIndex[i][j] = -1;
			}
		}
		BuildAnimeIndex(columns, row);
		MP = MaxMP = ATK = DEF = MAK = MDF = SMA = 0;
		HP = MaxHP = 1;
	}
	CHARACTOR(LPDIRECT3DTEXTURE9& sp, int Columns = 1, float Scaling = 1.0f, int Startframe = 0, int Endframe = 0, float DrawoffsetY = 0.85f)
	{
		D3DSURFACE_DESC surfaceDesc;
		int level = 0; //The level to get the width/height of (probably 0 if unsure)
					   //解释：texture分为几层surface，所以取了最上层
		sp->GetLevelDesc(level, &surfaceDesc);

		texSize[0] = surfaceDesc.Width;
		texSize[1] = surfaceDesc.Height;
		startframe = Startframe, endframe = Endframe;
		frame = 0, columns = Columns;
		row = (endframe + 1) / columns;
		width = surfaceDesc.Width / Columns, height = surfaceDesc.Height / row;
		scaling = Scaling, rotation = 0.0f;
		starttime = 0, delay = 66;//默认33帧
		velx = 0, vely = 0;
		color = D3DCOLOR_XRGB(255, 255, 255);
		center[0] = float(width*scaling*0.5), center[1] = float(height*scaling*DrawoffsetY);//很多精灵图像底部离纹理块底部大约是全高度的15%，因此上移50%-15%即到达所需位置
		x = -center[0], y = -center[1];
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				animeIndex[i][j] = -1;
			}
		}
		position[0] = position[1] = position[2] = 0.0f;
		MP = MaxMP = ATK = DEF = MAK = MDF = SMA = 0;
		HP = MaxHP = 1;
		faceDirection = 2;
		target[0] = target[1] = target[2] = -1;
		BuildAnimeIndex(row, columns);
		this->sprite = &sp;
	}
	CHARACTOR(SPECIES species, SPRITEINF sinformation, int lv = 1, D3DVECTOR pos = { 0.0f,0.0f,0.0f }, string nameTag = "",bool constantNPC=false)
	{
		//显示部分
		D3DSURFACE_DESC surfaceDesc;
		int level = 0; //The level to get the width/height of (probably 0 if unsure)
					   //解释：texture分为几层surface，所以取了最上层
		(*sinformation.spriteTex)->GetLevelDesc(level, &surfaceDesc);
		texSize[0] = surfaceDesc.Width;
		texSize[1] = surfaceDesc.Height;
		startframe = sinformation.Startframe, endframe = sinformation.Endframe;
		frame = 0, columns = sinformation.Columns;
		row = (endframe + 1) / columns;
		width = surfaceDesc.Width / sinformation.Columns, height = surfaceDesc.Height / row;
		scaling = sinformation.Scaling, rotation = 0.0f;
		starttime = 0, delay = sinformation.Delay;
		velx = 0, vely = 0;
		color = D3DCOLOR_XRGB(255, 255, 255);
		center[0] = float(width*scaling*0.5), center[1] = float(height*scaling*sinformation.DrawoffsetY);//很多精灵图像底部离纹理块底部大约是全高度的15%，因此上移50%-15%即到达所需位置
		x = -center[0], y = -center[1];
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				animeIndex[i][j] = -1;
			}
		}
		BuildAnimeIndex(row, columns);
		switch (sinformation.DirectionType)
		{
		case 0:
			//默认的上左下右顺序
			break;
		case 1:
			//常见的下左右上顺序
			animeIndex[0][0] = 3;
			animeIndex[1][0] = 1;
			animeIndex[2][0] = 0;
			animeIndex[3][0] = 2;
			break;
		default:
			break;
		}
		this->sprite = sinformation.spriteTex;
		icon = sinformation.spriteIcon;
		draw = sinformation.spriteDraw;
		//数据部分
		position[0] = pos.x, position[1] = pos.y, position[2] = pos.z;

		ATK = species.Strength*lv / BAS_ATTR_DIV_CON;
		DEF = (species.Stamina + species.Strength) / (2 * BAS_ATTR_DIV_CON);
		SMA = species.Stamina / BAS_ATTR_DIV_CON;
		MAK = species.MagicPower / BAS_ATTR_DIV_CON;
		MDF = (species.Stamina + species.MagicPower) / (2 * BAS_ATTR_DIV_CON);
		MP = MaxMP = species.MagicVolume / BAS_CON_ATTR_CON;
		HP = MaxHP = species.LifePower / BAS_CON_ATTR_CON;
		faceDirection = 2;
		target[0] = target[1] = target[2] = -1;
		tag = nameTag, level = lv, race = species.Name, profession = sinformation.name;
		constant = constantNPC;
	}
	/*
	举例：若是一个4个行动方向，每个方向9个精灵动画的纹理，则对应的动画索引为
	0	0	9
	1	9	9
	2	18	9
	3	27	9
	0	-1	-1
	-1	-1	-1
	-1	-1	-1
	-1	-1	-1（注意：-1的值是未赋初值的值，视编译器不同）
	可见，假如取第[3]行，则编号[2]方向的动画纹理初始偏移为[18]，一共[9]个。
	*/
	bool BuildAnimeIndex(int directionNum, int num1 = 0, int n2 = 0, int n3 = 0, int n4 = 0, int n5 = 0, int n6 = 0, int n7 = 0, int n8 = 0);
	bool Move(int InputDirection, float deltaTime);
	void CharaAnimate(float deltaTime);
	void DrawGUI();
	//获取最近目标
	CHARACTOR* GetNearbyChara(int range = 50);
	void Attack(CHARACTOR& target, ATTACKSKILL skill = { nullptr,1.0f,40,5 })
	{
		if (attackCD <= 0.0f)
		{
			int damage = ATK*skill.attackDamage / 20;
			int critical = rand() % 100;
			if (critical < skill.criticalRate)
				damage *= 1.5f;
			target.Guard(damage);
			attackCD = 1.0f;
		}
	}
	void Guard(int getDamage)
	{
		int finalDamage = getDamage - DEF;
		Damage(finalDamage);
	}
	bool Damage(int i = 0, int type = 0);
	bool Heal(int i = 0, int type = 0);
};

//伤害数字（一个）
struct DAMAGENUM
{
	vector<SPRITE> numbers;
	LPDIRECT3DTEXTURE9 NumTexSource;
	DAMAGENUM()
	{
		numbers.clear();
	}
	//伤害数字构建一个sprite
	DAMAGENUM(int damageNum,float x,float y,LPDIRECT3DTEXTURE9 Source)
	{
		vector<int> a;
		//是0则0
		if (damageNum == 0)
		{
			a.push_back(0);
		}
		//用数字组一个int数组（vector）
		else
		{
			while (damageNum > 0)
			{
				a.push_back(damageNum % 10);
				damageNum = damageNum / 10;
			}
			reverse(a.begin(), a.end());
		}
		//获取大小
		D3DSURFACE_DESC surfaceDesc;
		//源图像大小
		int texSize[2] = { 0,0 };
		//显示比例
		float scalexy[2] = { 0.0f,0.0f };
		int level = 0; //The level to get the width/height of (probably 0 if unsure)
					   //解释：texture分为几层surface，所以取了最上层
		NumTexSource = Source;
		NumTexSource->GetLevelDesc(level, &surfaceDesc);
		texSize[0] = surfaceDesc.Width;
		texSize[1] = surfaceDesc.Height;
		//填充numbers;
		numbers.clear();
		int index = 0;
		int dynamicShow = 0;
		for (auto i : a)
		{
			SPRITE s = SPRITE();
			s.width = texSize[0] / 12, s.height = texSize[1];
			s.x = x + (texSize[0] / 12)*index-(int)((int)a.size()*s.width*0.5f), s.y = y - texSize[1];
			s.frame = s.startframe = s.endframe = i;
			s.delay = DMG_SHOW_TIME;
			s.columns = 12;
			s.starttime = dynamicShow;
			numbers.push_back(s);
			index++;
			dynamicShow -= 50;
		}
		//numbers.swap(a);
	}
	void update(float deltaT)
	{
		
		for (auto iter = numbers.begin(); iter != numbers.end();)
		{
			//进时间
			iter->starttime += deltaT * 1000;
			if (iter->starttime > iter->delay)
			{
				iter = numbers.erase(iter);
				if (iter == numbers.end())
					break;
			}
			else
			{
				iter++;
			}
		}
	}
	void draw()
	{
		if (NumTexSource)
		{
			for each (auto var in numbers)
			{
				if (var.starttime > 0)
				{
					SpriteTransformDraw(NumTexSource, var.x, var.y-(var.starttime<200?var.starttime:200)/10, var.width, var.height, var.frame, var.columns);
				}
				
			}
		}
	}
};
//伤害显示组件
struct DAMAGENUMCOM
{
	vector<DAMAGENUM> damageObjects;
	LPDIRECT3DTEXTURE9 NumTexSource;
	void Init()
	{

	}
	void update(float deltaT)
	{
		for (auto iter = damageObjects.begin(); iter != damageObjects.end();)
		{
			iter->update(deltaT);
			if (iter->numbers.size()==0)
			{
				iter = damageObjects.erase(iter);
				if (iter == damageObjects.end())
					break;
			}
			else
			{
				iter++;
			}
		}
	}
	bool getNumTexture(string filepath)
	{
		NumTexSource = LoadTexture(filepath);
		if (!NumTexSource)
			return false;
		else
		{
			return true;
		}
	}
	bool addDamageNum(int damageNum, float x, float y)
	{
		DAMAGENUM d = DAMAGENUM(damageNum, x, y, NumTexSource);
		damageObjects.push_back(d);
		return true;
	}
};
//音频服务的基类
class Audio
{
public:
	Audio();
	//如果一个类有任何虚函数，它应该有一个虚析构函数——Effective C++ 来源：https://routinepanic.com/questions/when-to-use-virtual-destructors
	//我的理解：若基类析构函数不是虚函数，则析构一个父类指针指向的子类对象时，会调用父类的析构函数。因为虚函数可能导致内存空间的不同，
	//所以可能会内存泄漏。用虚拟析构函数会先调用子类析构函数，再调用父类析构函数。
	virtual ~Audio();
	virtual void playSound(string soundName) = 0;
	virtual void loopSound(string soundName) = 0;
	virtual void stopSound(string soundName) = 0;
	virtual void silenceSounds() = 0;
protected:
	map<string, CSound*> audioSources;
};

Audio::Audio()
{

}

Audio::~Audio()
{
	map<string, CSound*>(audioSources).swap(audioSources);
}
//空音频服务
class NullAudio :public Audio
{
	NullAudio()
	{

	}
	~NullAudio()
	{

	}
	//Do nothing
	/*
	override的使用要求：
	一虚：基类中，成员函数声明为虚拟的 (virtual)
	 	二容：基类和派生类中，成员函数的返回类型和异常规格 (exception specification) 必须兼容
		 	四同：基类和派生类中，成员函数名、形参类型、常量属性 (constness) 和 引用限定符 (reference qualifier) 必须完全相同
			注意要点：
			(1).覆盖的方法的标志必须要和被覆盖的方法的标志完全匹配，才能达到覆盖的效果；
			(2).覆盖的方法的返回值必须和被覆盖的方法的返回一致；
			(3).覆盖的方法所抛出的异常必须和被覆盖方法的所抛出的异常一致，或者是其子类；
			(4).被覆盖的方法不能为private，否则在其子类中只是新定义了一个方法，并没有对其进行覆盖。
			来源：https://blog.csdn.net/fanyun_01/article/details/79122136
			*/
	virtual void playSound(string soundName)override
	{

	}
	virtual void loopSound(string soundName)override
	{

	}
	virtual void stopSound(string soundName)override
	{

	}
	virtual void silenceSounds()override
	{

	}
};
//原作者版本的音频服务
class OriginAudio :public Audio
{
public:

	OriginAudio()
	{

	}
	~OriginAudio()
	{

	}
	void addSound(string name, CSound* sound)
	{
		audioSources[name] = sound;
	}
	virtual void playSound(string soundName)override
	{
		CSound* audio = NULL;
		try
		{
			audio = audioSources.at(soundName);
			audio->Play();
		}
		catch (const std::out_of_range& e)
		{
			//没找到对应的音乐资源
			std::cerr << e.what() << '\n' << soundName << " was not found." << std::endl;
		}

	}
	virtual void loopSound(string soundName)override
	{
		CSound* audio = NULL;
		try
		{
			audio = audioSources.at(soundName);
			LoopSound(audio);
		}
		catch (const std::out_of_range& e)
		{
			//没找到对应的音乐资源
			std::cerr << e.what() << '\n' << soundName << " was not found." << std::endl;
		}
	}
	virtual void stopSound(string soundName)override
	{
		CSound* audio = NULL;
		try
		{
			audio = audioSources.at(soundName);
			audio->Stop();
			audio->Reset();
		}
		catch (const std::out_of_range& e)
		{
			//没找到对应的音乐资源
			std::cerr << e.what() << '\n' << soundName << " was not found." << std::endl;
		}
	}
	virtual void silenceSounds()override
	{
		for (auto i = audioSources.begin(); i != audioSources.end(); i++)
		{
			if (i->second != NULL)
			{
				i->second->Stop();
			}
		}
	}

};
//音频服务定位器
class AudioLocator
{
public:
	AudioLocator();
	~AudioLocator();
	static Audio* getAudio()
	{
		return service_;
	}
	static void setService(Audio* service)
	{
		service_ = service;
	}
private:
	static Audio* service_;
};

Audio* AudioLocator::service_ = NULL;//静态变量应在类的生命之外定义一次，因为它和类一个初始化级别

AudioLocator::AudioLocator()
{
}

AudioLocator::~AudioLocator()
{
}

class CharaLocator
{
public:
	static const CHARACTOR* getChara(const string charaName)
	{
		for (auto i = charaList->begin(); i != charaList->end(); i++)
		{
			if (charaName == i->tag)
			{
				return &(*i);
			}
		}
		return nullptr;
	}
	static const vector<CHARACTOR>& getService()
	{
		return *charaList;
	}
	static void setService(const vector<CHARACTOR>* service)
	{
		charaList = service;
	}
private:
	static const vector<CHARACTOR>* charaList;
};

const vector<CHARACTOR>* CharaLocator::charaList = NULL;

//事件
struct EVENT
{
	
};
//任务的种类(事件类型）
enum MissionEvent
{
	EVENT_NONE,
	EVENT_KILL_MONSTER,
	EVENT_COLLECT_ITEM,
	EVENT_TRANSFER_ITEM,
	EVENT_INJURED,
	EVENT_ARRIVE,
	EVENT_INGAME,
	EVENT_ATLOCATION
};
//任务的状态
enum MissionState
{
	NotAccept,//未接受
	Underway,//做任务
	Complete,//完成（未交任务）
	Finish//结束
};
//任务目标
struct MISSIONTARGET
{
	int number;//应达到数量
	int count;//计数器
	string name;//目标名称
	MISSIONTARGET(string Name, int n)
	{
		number = n, count = 0, name = Name;
	}
};

struct TALK
{
	string CharaName;
	string message;
	enum Sentence {
		Talking,
		Missioning
	};
	Sentence sentence;
	TALK(string name, string msg, Sentence s = Sentence::Talking)
	{
		CharaName = name;
		message = msg;
		sentence = s;
	}
	
};
//任务
struct MISSION
{
	int ID;//任务ID
	string starter;//触发任务的NPC
	string receiver;//完成任务的NPC
	MissionEvent eventType;//任务类型
	MissionState eventState;//任务状态
	string name;//任务名字
	string description;//任务描述
	vector<MISSIONTARGET> targets;//任务目标
	map<string, vector<TALK>> talks[2];
	MISSION()
	{
		ID = -1;
		starter = "";
		eventType = MissionEvent::EVENT_NONE;
		eventState = MissionState::NotAccept;
		name = "", description = "";
	}
	MISSION(int id, string starterName, MissionEvent Type, string n, string d, vector<MISSIONTARGET> m, MissionState State = NotAccept)
	{
		ID = id;
		starter = starterName;
		eventType = Type;
		eventState = State;
		name = n, description = d;
		//将区间[first,last)的元素赋值到当前的vector容器中，或者赋n个值为x的元素到vector容器中，这个容器会清除掉vector容器中以前的内容
		targets.assign(m.begin(), m.end());
	}
};

struct Button
{
	int x, y;
	int width, height;
	LPDIRECT3DTEXTURE9 backGround;
	string name;
	string text;
	Button()
	{
		x = y = width = height = 0;
		backGround = NULL;
		name = "blank", text = "";
	}
	Button(int x1, int y1, int w, int h, LPDIRECT3DTEXTURE9 BG, string n = "null", string t = "")
	{
		x = x1, y = y1, width = w, height = h;
		backGround = BG;
		name = n, text = t;
	}
	bool click(int mx,int my);
};

enum DialogType {
	charaStatus,
	charaState,
	messageBox,
	fullScreenBox,
	hintBox,
	worldmap
};

struct DIATEX
{
	LPDIRECT3DTEXTURE9 tex;
	int x, y;
	int width, height;
	int frame, columns;
	DIATEX(LPDIRECT3DTEXTURE9 texture = nullptr, int x1 = 0, int y1 = 0, int w = 0, int h = 0, int f = 0, int c = 1)
	{
		tex = texture;
		x = x1, y = y1;
		width = w, height = h;
		frame = f, columns = c;
	}
};

struct DIASTR
{
	string str;
	int x, y;
	int width, height;
	D3DCOLOR color;
	DIASTR(string s = "", int x1 = 0, int y1 = 0, D3DCOLOR c = D3DCOLOR_XRGB(255, 255, 255), int w = -1, int h = -1)
	{
		str = s;
		x = x1, y = y1;
		width = w, height = h;
		color = c;
	}
};
//将输入的图像按给定的大小和位置缩放
void DialogTransformDraw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, float rotation = 0, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255))
{
	//获取大小
	D3DSURFACE_DESC surfaceDesc;
	//源图像大小
	int texSize[2] = { 0,0 };
	//显示比例
	float scalexy[2] = { 0.0f,0.0f };
	int level = 0; //The level to get the width/height of (probably 0 if unsure)
				   //解释：texture分为几层surface，所以取了最上层
	image->GetLevelDesc(level, &surfaceDesc);
	texSize[0] = surfaceDesc.Width;
	texSize[1] = surfaceDesc.Height;
	//求显示比例(按输入的大小来缩放）
	scalexy[0] = (float)width / texSize[0], scalexy[1] = (float)height / texSize[1];
	D3DXVECTOR2 scale(scalexy[0], scalexy[1]);//xy方向缩放
	D3DXVECTOR2 trans((float)x, (float)y);//xy偏移
	D3DXVECTOR2 center((float)(width) / 2, (float)(height) / 2);//旋转中心
	D3DXMATRIX mat;//变换矩阵
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);//设定变换矩阵
	spriteobj->SetTransform(&mat);
	RECT srcRect = { 0, 0, texSize[0], texSize[1] };
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);//绘制
	D3DXMatrixIdentity(&mat);//创建单位矩阵
	spriteobj->SetTransform(&mat);//恢复正常坐标系
}

struct DIALOG
{
	union DIALOG_POS
	{
		float covPos[4];
		int absPos[4];
	};
	DialogType type;
	map<string, DIASTR> sentences;
	map<string, DIATEX> textures;
	vector <Button> buttons;
	LPD3DXFONT font;
	static LPDIRECT3DTEXTURE9 msgBoxBackGround;
	static LPDIRECT3DTEXTURE9 statusBoxBackGround;
	static LPDIRECT3DTEXTURE9 statusBoxCover;
	static LPDIRECT3DTEXTURE9 fullscreenBlackBackGround;
	static LPDIRECT3DTEXTURE9 statusHP;
	static LPDIRECT3DTEXTURE9 statusMP;
	DIALOG_POS pos;
	static float LastDialogTime;
	DIALOG()
	{

	}
	DIALOG(DialogType t, map<string, DIASTR> ss, map<string, DIATEX> texs, vector <Button> bs, LPD3DXFONT f = NULL)
	{
		type = t;
		sentences = ss;
		textures = texs;
		buttons = bs;
		if (f != NULL)
		{
			font = f;
		}
	}
	DIALOG(DialogType t, const CHARACTOR& chara, string sentence = "")
	{
		type = t;
		font = MakeFont("TimesNewRoman", 40);
		if (t == charaStatus)
		{
			float HPratio = (float)chara.HP / (float)chara.MaxHP;
			float MPratio = (float)chara.MP / (float)chara.MaxMP;
			pos.absPos[0] = 20, pos.absPos[1] = 20, pos.absPos[2] = 340, pos.absPos[3] = 180;
			textures["backGround"] = DIATEX(statusBoxBackGround, pos.absPos[0], pos.absPos[1], pos.absPos[2] - pos.absPos[0], pos.absPos[3] - pos.absPos[1]);
			textures["cover"] = DIATEX(statusBoxCover, pos.absPos[0], pos.absPos[1], pos.absPos[2] - pos.absPos[0], pos.absPos[3] - pos.absPos[1]);
			textures["Icon"] = DIATEX(chara.icon, 40, 40, 100, 100);
			textures["HP"] = DIATEX(DIALOG::statusHP, 160, 40, (int)(160 * HPratio), 30);
			textures["MP"] = DIATEX(DIALOG::statusMP, 160, 80, (int)(160 * MPratio), 30);
		}
		else if (t == charaState)
		{
			pos.absPos[0] = 0, pos.absPos[1] = 0, pos.absPos[2] = 0, pos.absPos[3] = 0;
			textures["backGround"] = DIATEX(msgBoxBackGround, chara.position[0] - SCREENW - 32, chara.position[1] - SCREENH - 50, 64, 100);
		}
		else if (t == messageBox)
		{
			int dialogPositionY = 0.6*SCREENH, IconSize = SCREENH*0.13;
			pos.covPos[0] = 0.0f, pos.covPos[1] = 0.6f, pos.covPos[2] = 1.0f, pos.covPos[3] = 0.4f;
			textures["backGround"] = DIATEX(msgBoxBackGround, 0, dialogPositionY, SCREENW, SCREENH - dialogPositionY);
			textures["leftIcon"] = DIATEX(chara.icon, IconSize*0.2, dialogPositionY - IconSize / 2, IconSize, IconSize);
			sentences["mainTalk"] = DIASTR(sentence, IconSize*0.5, dialogPositionY + IconSize, D3DCOLOR_XRGB(255, 255, 255));
		}
		else if (t == DialogType::fullScreenBox)
		{
			textures["backGround"] = DIATEX(fullscreenBlackBackGround, 0, 0, SCREENW, SCREENH);
			pos.absPos[0] = 0, pos.absPos[1] = 0, pos.absPos[2] = SCREENW, pos.absPos[3] = SCREENH;
			sentences["mainTalk"] = DIASTR(sentence, (int)(SCREENW*0.2), (int)(SCREENH*0.618), D3DCOLOR_XRGB(255, 255, 255));
		}
		else if (t == hintBox)
		{

		}
		else if (t == DialogType::worldmap)
		{
			pos.covPos[0] = 0.05f, pos.covPos[1] = 0.05f, pos.covPos[1] = 0.95f, pos.covPos[1] = 0.95f;
			textures["backGround"] = DIATEX(msgBoxBackGround, 0, 0, SCREENW, SCREENH);
		}
	}
	void DrawFonts()
	{
		//auto font = MakeFont("TimesNewRoman", 40);
		//map<string, DIASTR>::iterator iter;
		//for (iter = sentences.begin(); iter != sentences.end(); iter++)
		//{
		//	FontPrint(font, iter->second.x, iter->second.y, iter->second.str, iter->second.color);
		//}
		for each (auto var in sentences)
		{
			FontPrint(font, var.second.x, var.second.y, var.second.str, var.second.color);
		}

	}
	void DrawFonts(int x, int y)
	{
		//auto font = MakeFont("TimesNewRoman", 40);
		//map<string, DIASTR>::iterator iter;
		//for (iter = sentences.begin(); iter != sentences.end(); iter++)
		//{
		//	FontPrint(font, iter->second.x, iter->second.y, iter->second.str, iter->second.color);
		//}
		for each (auto var in sentences)
		{
			FontPrint(font, x, y, var.second.str, var.second.color);
		}

	}
	void DrawTexs()
	{
		for each (auto var in textures)
		{
			if (var.second.tex != nullptr)
			{
				DialogTransformDraw(var.second.tex, var.second.x, var.second.y, var.second.width, var.second.height);
			}
		}
	}
	void DrawBtns()
	{
		for each (auto var in buttons)
		{
			if (var.backGround != nullptr)
			{
				DialogTransformDraw(var.backGround, var.x, var.y, var.width, var.height);
			}
		}
	}
	void show()
	{
		
		if (type == charaStatus)
		{
			DialogTransformDraw(textures.at("backGround").tex, textures.at("backGround").x, textures.at("backGround").y, textures.at("backGround").width, textures.at("backGround").height);
			DialogTransformDraw(textures.at("Icon").tex, textures.at("Icon").x, textures.at("Icon").y, textures.at("Icon").width, textures.at("Icon").height);
			DialogTransformDraw(textures.at("HP").tex, textures.at("HP").x, textures.at("HP").y, textures.at("HP").width, textures.at("HP").height);
			DialogTransformDraw(textures.at("MP").tex, textures.at("MP").x, textures.at("MP").y, textures.at("MP").width, textures.at("MP").height);
			DialogTransformDraw(textures.at("cover").tex, textures.at("cover").x, textures.at("cover").y, textures.at("cover").width, textures.at("cover").height);
			DrawFonts();
		}
		else if (type == charaState)
		{

		}
		else if (type == messageBox)
		{
			int dialogPositionY = 0.6*SCREENH, IconSize = SCREENH*0.13;
			try
			{
				DialogTransformDraw(textures.at("backGround").tex, 0, dialogPositionY, SCREENW, SCREENH - dialogPositionY);
				DialogTransformDraw(textures.at("leftIcon").tex, IconSize*0.2, dialogPositionY - IconSize / 2, IconSize, IconSize);
				auto font = MakeFont("Arial", 36);
				FontPrint(font, IconSize*0.5, dialogPositionY + IconSize, sentences.at("mainTalk").str, D3DCOLOR_XRGB(255, 255, 255));
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << e.what() << '\n' << " had occurd." << std::endl;
			}
		}
		else if (type == DialogType::fullScreenBox)
		{
			DrawTexs();
			DrawBtns();
			DrawFonts();
		}
	}
	bool isClicked(int x, int y)
	{
		int left = 0;
		int up = 0;
		int right = 0;
		int down = 0;
		RECT r;
		switch (type)
		{
			//绝对大小的类型
		case DialogType::charaState:
		case DialogType::charaStatus:
		case DialogType::hintBox:
		case DialogType::fullScreenBox:
			left = pos.absPos[0], up = pos.absPos[1], right = pos.absPos[2], down = pos.absPos[3];
			r = { left,up,right,down };
			break;
			//相对大小的类型
		case DialogType::messageBox:
		case DialogType::worldmap:
			left = (int)(pos.covPos[0] * SCREENW), up = (int)(pos.covPos[1] * SCREENH), right = left + (int)(pos.covPos[2] * SCREENW), down = up + (int)(pos.covPos[3] * SCREENH);
			r = { left,up,right,down };
			break;
		default:
			break;
		}
		return POINTInside(x, y, r);
	}
	void update(CHARACTOR& chara)
	{
		if(type==DialogType::charaStatus)
		{
			float HPratio = (float)chara.HP / (float)chara.MaxHP;
			float MPratio = (float)chara.MP / (float)chara.MaxMP;
			textures["HP"].width = (int)(160 * HPratio);
			textures["MP"].width = (int)(160 * MPratio);
		}
		else if (type == DialogType::fullScreenBox)
		{
			textures.at("backGround").width = SCREENW, textures.at("backGround").height = SCREENH;
			if (buttons.size() > 0)
			{
				buttons[0].x = (SCREENW - 200) / 2, buttons[0].y = (SCREENH - 90) / 2;
			}
			//textures.at("cover").width = SCREENW, textures.at("cover").height = SCREENH;
		}
	}
};
//静态成员的类外声明
float DIALOG::LastDialogTime = 0.0f;
LPDIRECT3DTEXTURE9 DIALOG::msgBoxBackGround = NULL;
LPDIRECT3DTEXTURE9 DIALOG::statusBoxBackGround = NULL;
LPDIRECT3DTEXTURE9 DIALOG::statusBoxCover = NULL;
LPDIRECT3DTEXTURE9 DIALOG::fullscreenBlackBackGround = NULL;
LPDIRECT3DTEXTURE9 DIALOG::statusHP = NULL;
LPDIRECT3DTEXTURE9 DIALOG::statusMP = NULL;

void Mission0action();
void Mission1action();

//观察者基类
class Observer
{
public:
	Observer();
	~Observer();
	virtual void onNotify(const CHARACTOR&chara,MissionEvent event)=0;
	string getObserverName()
	{
		return observerName;
	}
protected:
	string observerName;
};

Observer::Observer()
{
}

Observer::~Observer()
{
}

void Observer::onNotify(const CHARACTOR&chara, MissionEvent event)
{
	//普通虚函数同时继承接口和缺省实现是危险的，最好是基类中实现缺省行为 (behavior)，但只有在派生类要求时才提供该缺省行为
	//子类中可以用Observer::onNotify(chara,e)的形式显式调用缺省实现，但不会自动调用
}

//观察者一号：任务系统
class EventSystem :public Observer//struct也能继承class
{
public:
	EventSystem()
	{
		observerName = "EventSystem";
	}
	virtual void onNotify(const CHARACTOR&chara, MissionEvent event)
	{
		//检查玩家任务列表
		for (auto var = playerMissions.begin(); var < playerMissions.end(); var++)
		{
			//跳过完成任务？
			if (var->eventState == MissionState::Finish)
				continue;
			//检查任务类型
			switch (event)
			{
			case EVENT_KILL_MONSTER:
				if (var->eventType == EVENT_KILL_MONSTER)
				{
					for(auto target = var->targets.begin(); target < var->targets.end(); target++)
					{
						if (target->name == chara.tag)
						{
							target->count++;//这个count不是原数据，你加个什么劲？（现在是了）
							if (target->count >= target->number)
							{
								var->eventState = MissionState::Complete;
								AudioLocator::getAudio()->playSound("missionComplete");
							}
						}
					}
				}
				break;
			case EVENT_COLLECT_ITEM:
				if (var->eventType == EVENT_COLLECT_ITEM)
				{

				}
				break;
			case EVENT_TRANSFER_ITEM:
				if (var->eventType == EVENT_TRANSFER_ITEM)
				{

				}
				break;
			case EVENT_INGAME:
				if (var->eventType == EVENT_INGAME)
				{
					if (var->eventState == MissionState::NotAccept)
					{
						Mission0action();
						//改变任务状态
						var->eventState = MissionState::Finish;
					}
				}
				break;
			case EVENT_ATLOCATION:
				if (var->eventType == EVENT_ATLOCATION)
				{
					if (var->eventState == MissionState::NotAccept&&chara.currTerrian == "Snowfield")
					{
						Mission1action();
						//改变任务状态
						var->eventState = MissionState::Finish;
					}
				}
				break;
			default:
				break;
			}
		}
		
	}
	void setMissions(vector<MISSION>& ms)
	{
		playerMissions.insert(playerMissions.end(), ms.begin(), ms.end());//插入，相比assign()不会清除原来的内容
	}
	int getKillNum()
	{
		if ((int)playerMissions.size() > 0)
		{
			return playerMissions[0].targets[0].count;
		}
		else
		{
			return 0;
		}
	}
	int Size()
	{
		return (int)playerMissions.size();
	}
	MISSION& getMissionByName(string name)
	{
		//先检查完成任务后检查开始任务（receiver和starter）
		//for (auto m: playerMissions)
		for (auto m = playerMissions.begin(); m < playerMissions.end(); m++)
		{
			if (m->receiver == name&&m->eventState != MissionState::Finish)//
			{
				return *m;
			}
			if (m->starter == name&&m->eventState!=MissionState::Finish)
			{
				return *m;
			}
		}
		return playerMissions[0];
	}
	MISSION& getMissionByID(const int id)
	{
		for (auto m = playerMissions.begin(); m < playerMissions.end(); m++)
		{
			if (id == m->ID)
			{
				return *m;
			}
		}
		return playerMissions[0];
	}
protected:
private:
	vector<MISSION> playerMissions;
};

class MissionSystemLocator
{
public:
	MissionSystemLocator();
	~MissionSystemLocator();
	static EventSystem* getMissionSystem()
	{
		return service_;
	}
	static void setService(EventSystem* service)
	{
		service_ = service;
	}
private:
	static EventSystem* service_;
};

EventSystem* MissionSystemLocator::service_ = NULL;

MissionSystemLocator::MissionSystemLocator()
{
}

MissionSystemLocator::~MissionSystemLocator()
{
}

//被观察者基类
class Subject
{
public:
	bool addObserver(Observer* observer);
	bool removeObserver(Observer* observer);
protected:
	void notify(const CHARACTOR&chara, MissionEvent e);
private:
	vector<Observer*> observers;

};

inline bool Subject::addObserver(Observer * observer)
{
	bool in = true;
	for each (auto var in observers)
	{
		//防止同一观察者重复添加
		if (var->getObserverName() == observer->getObserverName())
		{
			in = false;
		}
	}
	if (in)
	{
		observers.push_back(observer);
	}
	return false;
}

inline bool Subject::removeObserver(Observer * observer)
{
	return false;
}

void Subject::notify(const CHARACTOR & chara, MissionEvent e)
{
	for (auto var = observers.begin(); var < observers.end(); var++)
	{
		(*var)->onNotify(chara, e);
	}
}
//被观察者一号：赫尔墨斯（信使，指其代main()行使发送信号的职责）
class Hermes:public Subject
{
public:
	Hermes();
	~Hermes();
	void notify(const CHARACTOR&chara, MissionEvent e);
private:

};

Hermes::Hermes()
{
	
}

Hermes::~Hermes()
{
}

inline void Hermes::notify(const CHARACTOR & chara, MissionEvent e)
{
	Subject::notify(chara, e);
}

//观察者二号：音频系统
class AudioManager:public Observer
{
public:
	AudioManager();
	~AudioManager();
	virtual void onNotify(const CHARACTOR&chara, MissionEvent event)
	{
		switch (event)
		{
		case EVENT_KILL_MONSTER:
			AudioLocator::getAudio()->playSound("NormalDead");
			break;
		case EVENT_INJURED:
			AudioLocator::getAudio()->playSound("SwordSmall");
			break;
		case EVENT_ATLOCATION:
			if (chara.lastTerrian == "null")
			{

			}
			else if (chara.lastTerrian == "Village")
			{
				AudioLocator::getAudio()->stopSound("villageBGM");
			}
			else if (chara.lastTerrian == "Snowfield")
			{
				AudioLocator::getAudio()->stopSound("icefieldBGM");
			}
			else if (chara.lastTerrian == "Seaside")
			{
				AudioLocator::getAudio()->stopSound("seasideBGM");
			}
			else if (chara.lastTerrian == "Forest"|| chara.lastTerrian == "Temple")
			{
				AudioLocator::getAudio()->stopSound("forestBGM");
			}
			if (chara.currTerrian == "Village")
			{
				AudioLocator::getAudio()->loopSound("villageBGM");
			}
			else if (chara.currTerrian == "Snowfield")
			{
				AudioLocator::getAudio()->loopSound("icefieldBGM");
			}
			else if (chara.currTerrian == "Seaside")
			{
				AudioLocator::getAudio()->loopSound("seasideBGM");
			}
			else if (chara.currTerrian == "Forest" || chara.currTerrian == "Temple")
			{
				AudioLocator::getAudio()->loopSound("forestBGM");
			}
			break;
		default:
			break;
		}
	}
private:

};

AudioManager::AudioManager()
{
	observerName = "AudioManager";
}

AudioManager::~AudioManager()
{
}

D3DCOLOR getSceneColorFromTime(tm* lotm)
{
	D3DCOLOR color;
	switch (lotm->tm_hour)
	{
		//凌晨
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		color = D3DCOLOR_XRGB(255, 255, 255);
		break;
		//早上
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		color = D3DCOLOR_XRGB(255, 255, 255);
		break;
		//中午
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		color = D3DCOLOR_XRGB(255, 255, 255);
		break;
		//下午
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
		color = D3DCOLOR_XRGB(255, 255, 255);
		break;
		//晚上
	case 21:
	case 22:
	case 23:
		color = D3DCOLOR_XRGB(255, 255, 255);
		break;
	case 24://24???
	default:
		color = D3DCOLOR_XRGB(255, 255, 255);
		break;
	}
	string s = to_string(lotm->tm_hour) + ":" + to_string(lotm->tm_min) + ":" + to_string(lotm->tm_sec);
	return D3DCOLOR_XRGB(255, 255, 255);
}


#endif //!INGAMEOBJECT_H