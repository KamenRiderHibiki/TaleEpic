

#include "DirectX.h"
#include "InGameObject.h"

int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

#define MAP_SOURCE_NUM 10		//地图资源图像数量
#define PARTICLE_NUMBER  200	//粒子数量
#define TILE_PIXEL_NUM 32		//每个瓦片的宽度（像素）

#define DIRECTION_UP 0			//上
#define DIRECTION_LEFT 1		//左
#define DIRECTION_DOWN 2		//下
#define DIRECTION_RIGHT 3		//右

const string APPTITLE = "Test Zone";//标题
int SCREENW = 1024;//屏幕初始宽度
int SCREENH = 768;//屏幕初始高度
int ScrollX = 0, ScrollY = 0;//卷轴地图的左上角位置
int OffsetX = 0, OffsetY = 0;//地图绘制缓存的左上角位置
int MaxMapX = 0, MaxMapY = 0;//地图边界
int wildEnemyNum = 0;
double wildSpawnCD = EMY_SPN_CD;
GameState gamestate = GameState::Init;
bool gameStart = false;
bool snowShow = false;

//地图块信息
struct MAPTILEINF
{
	int width, height;
	LPDIRECT3DTEXTURE9* tex;
	vector<short> obstacles;
	string terrian;
	MAPTILEINF()
	{
		width = height = 0;
		tex = NULL;
		terrian = "";
	}
};

//雪花粒子结构体的定义
struct PARTICLE
{
	float x, y;      //坐标位置
	float width, height,maxWidth,maxHeight;//大小
	float RotationY;         //雪花绕自身Y轴旋转角度
	float RotationX;       //雪花绕自身X轴旋转角度
	float FallSpeed;       //雪花下降速度
	float WindSpeed;       //雪花旋转速度
	float scale;           //比例
	int   TextureIndex;     //纹理索引编号
	PARTICLE()
	{
		x = y = width = height = RotationX = RotationY = WindSpeed = 0.0f;
		TextureIndex = 0;
		scale = 1.0f;
	}
};

//粒子系统类的定义
class ParticleClass
{
private:
	PARTICLE m_Snows[PARTICLE_NUMBER];    //粒子数组
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;      //保存粒子数据的顶点缓存
	LPDIRECT3DTEXTURE9 m_pTexture[6];  //雪花纹理

public:
	ParticleClass(LPDIRECT3DDEVICE9 d3ddev);   //构造函数
	~ParticleClass();                                  //析构函数
	HRESULT InitParticle(HWND window);        //粒子系统初始化函数
	HRESULT UpdateParticle(float fElapsedTime);   //粒子系统更新函数
	HRESULT RenderParticle();   //粒子系统渲染函数
};

ParticleClass::ParticleClass(LPDIRECT3DDEVICE9 d3ddev)
{

}

ParticleClass::~ParticleClass()
{
}

//粒子系统初始化函数
HRESULT ParticleClass::InitParticle(HWND window)
{
	D3DXVECTOR2 size[6];
	//创建6种雪花纹理
	for (int i = 0; i < 6; i++)
	{
		string filepath = "snow" + to_string(i) + ".jpg";
		m_pTexture[i] = LoadTexture(filepath);
		if (!m_pTexture[i])
		{
			MessageBox(window, "Error load particle images", "Error!", MB_OK | MB_ICONERROR);
		}
		SPRITE sp = GetSpriteSize(filepath, &sp, 1, 1);
		size[i].x = (float)sp.width, size[i].y = (float)sp.height;
	}
	//初始化雪花粒子数组
	srand(GetTickCount());
	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		m_Snows[i].x = float(rand() % SCREENW)+ScrollX;
		m_Snows[i].y = float(rand() % SCREENH)+ScrollY;
		m_Snows[i].RotationY = (rand() % 100) / 5.0f;
		m_Snows[i].RotationX = (rand() % 100) / 5.0f;
		m_Snows[i].FallSpeed = 30.0f + rand() % 10;
		m_Snows[i].WindSpeed = -20.0f + rand() % 100 / 10.0f;
		m_Snows[i].TextureIndex = rand() % 6;
		m_Snows[i].maxHeight = m_Snows[i].height = size[m_Snows[i].TextureIndex].y;
		m_Snows[i].maxWidth = m_Snows[i].width = size[m_Snows[i].TextureIndex].x;
		m_Snows[i].scale = rand() % 100 / 1000.0f;
	}
	

	return S_OK;
}

//粒子系统更新函数
HRESULT ParticleClass::UpdateParticle(float fDeltaTime)
{
	//一个for循环，更新每个雪花粒子的当前位置和角度
	for (int i = 0; i<PARTICLE_NUMBER; i++)
	{
		m_Snows[i].y += m_Snows[i].FallSpeed*fDeltaTime;
		m_Snows[i].x += m_Snows[i].WindSpeed*fDeltaTime;
		//更新位置
		//如果雪花粒子落到地面, 重新将其高度设置为最大
		if (m_Snows[i].y > SCREENH+ScrollY)
		{
			m_Snows[i].y -= SCREENH;
		}
		if (m_Snows[i].y < ScrollY)
		{
			m_Snows[i].y += SCREENH;
		}
		if (m_Snows[i].x > SCREENW+ScrollX)
		{
			m_Snows[i].x -= SCREENW;
		}
		if (m_Snows[i].x < 0+ScrollX)
		{
			m_Snows[i].x += SCREENW;
		}
		//更新大小
		m_Snows[i].width -= m_Snows[i].RotationX*fDeltaTime;
		if (m_Snows[i].width < 0)
		{
			m_Snows[i].width = -m_Snows[i].width;
			m_Snows[i].RotationX = -m_Snows[i].RotationX;
		}
		if (m_Snows[i].width > m_Snows[i].maxWidth)
		{
			m_Snows[i].width -= m_Snows[i].width - m_Snows[i].maxWidth;
			m_Snows[i].RotationX = -m_Snows[i].RotationX;
		}

		m_Snows[i].height -= m_Snows[i].RotationY*fDeltaTime;
		if (m_Snows[i].height < 0)
		{
			m_Snows[i].height = -m_Snows[i].height;
			m_Snows[i].RotationY = -m_Snows[i].RotationY;
		}
		if (m_Snows[i].height > m_Snows[i].maxHeight)
		{
			m_Snows[i].height -= m_Snows[i].height - m_Snows[i].maxHeight;
			m_Snows[i].RotationY = -m_Snows[i].RotationY;
		}
		/*
		if (flag)
		{
			srand(GetTickCount());
			int j = rand() % 4;
			if (j == 1)
			{
				//m_Snows[i].y = 0;
			}
		}
		*/
		
	}

	return S_OK;
}

//“粒子”绘制
void ParticleDraw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, float rotation, float scaling1, float scaling2, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255))
{
	D3DXVECTOR2 scale(scaling1, scaling2);//xy方向缩放
	D3DXVECTOR2 trans(x+(width-width*scaling1)/2, y+(height-height*scaling2)/2);//xy偏移
	D3DXVECTOR2 center((float)(width*scaling1) / 2, (float)(height*scaling2) / 2);//旋转中心
	D3DXMATRIX mat;//变换矩阵
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);//设定变换矩阵
	spriteobj->SetTransform(&mat);
	int fx = 0;
	int fy = 0;
	RECT srcRect = { fx, fy, fx + width, fy + height };
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);

	D3DXMatrixIdentity(&mat);//创建单位矩阵
	spriteobj->SetTransform(&mat);//恢复正常坐标系
}

HRESULT ParticleClass::RenderParticle()
{
	//渲染雪花
	for (int i = 0; i<PARTICLE_NUMBER; i++)
	{
		ParticleDraw(m_pTexture[m_Snows[i].TextureIndex], (int)(m_Snows[i].x-ScrollX- m_Snows[i].maxWidth/2), (int)(m_Snows[i].y-ScrollY - m_Snows[i].maxHeight / 2), (int)m_Snows[i].maxWidth, (int)m_Snows[i].maxHeight,0.0f, (m_Snows[i].width/ m_Snows[i].maxWidth)*m_Snows[i].scale, (m_Snows[i].height/ m_Snows[i].maxHeight)*m_Snows[i].scale);
	}

	return S_OK;
}

//地图索引信息
struct MAPINDEX
{
	int number = 0;//地块序号
	int tilenum = 0;//地块类型
	MAPINDEX() = delete;
	MAPINDEX(int m, int s)
	{
		number = m;
		tilenum = s;
	}
};
//地图区块信息
struct MAPBLOCK
{
	int width, height;
	int x, y;
	int mapSourceIndex;
	vector <MAPINDEX> mapindex;
	MAPBLOCK()
	{
		width = height = 0;
		x = y = 0;
	}
	MAPBLOCK(int ix, int iy, int offsetX = 0, int OffsetY = 0)
	{
		x = offsetX, y = OffsetY;
		width = ix, height = iy;
	}
	MAPBLOCK(const int in[])
	{
		width = in[0], height = in[2], x = in[2], y = in[3];
	}
	~MAPBLOCK()
	{
		vector<MAPINDEX>(mapindex).swap(mapindex);
	}
	RECT getRECT()
	{
		RECT result = { x,y,x + width*TILE_PIXEL_NUM,y + height*TILE_PIXEL_NUM };
		return result;
	}
};
//地图区块的集合
struct MAP
{
	vector <MAPBLOCK> maplist;
	MAP()
	{

	}
	MAP(vector<MAPBLOCK> mapList)
	{
		maplist = mapList;
	}
	~MAP()
	{
		vector<MAPBLOCK>(maplist).swap(maplist);
	}
	MAP operator+(const MAP& map)
	{
		maplist.insert(maplist.end(), map.maplist.begin(), map.maplist.end());
		return *this;
	}
	MAP& operator=(const MAP& map)
	{
		maplist.assign(map.maplist.begin(), map.maplist.end());
		return *this;
	}
	MAP& operator+=(const MAP& map)
	{
		maplist.insert(maplist.end(), map.maplist.begin(), map.maplist.end());
		return *this;
	}
};
//种族及其权
struct SPECIESPRO
{
	float probability;
	string speciesName;
	SPECIESPRO(string n = "", float p = 0.0f)
	{
		speciesName = n;
		probability = p;
	}
};


//判断能否移动
bool IsPositionMoveable(float x, float y, CHARACTOR* chara = nullptr);

string getSpeciesFromTerrian(int x, int y);
//绘制角色
bool DrawCharacters(vector<CHARACTOR> c)
{
	for each (CHARACTOR var in c)
	{
		SpriteTransformDraw(*var.sprite, (int)(var.x + var.position[0] - ScrollX), (int)(var.y + var.position[1] - ScrollY), var.width, var.height, var.frame, var.columns, 0.0f, var.scaling);
		var.DrawGUI();
	}
	return false;
}

//gbk和utf8互相转换 来源：https://blog.csdn.net/u012234115/article/details/83186386
string GbkToUtf8(const char *src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

string Utf8ToGbk(const char *src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

map<string, SPECIES> loadSpeciesMapFronJson(string filepath)
{
	map<string, SPECIES> result;
	//读取文件
	ifstream ReadFile;
	ReadFile.open(filepath);
	if (ReadFile.fail())
	{
		cout << "open json file failed." << endl;
		return result;
	}
	//装载数据
	json j;
	ReadFile >> j;
	//读取版本
	float version = j["version"];
	cout << version << endl;
	//读取所有种族数据
	for (int i = 0; i < (int)j["species"].size(); i++)
	{
		auto j0 = j["species"][i];
		string n = j0["name"], p = j0["prototype"];
		n = Utf8ToGbk(n.c_str()), p = Utf8ToGbk(p.c_str());
		SPECIES sp = SPECIES(i, j0["strength"], j0["stamina"], j0["magicpower"], j0["mana"], j0["intelligence"], j0["agile"], j0["lifepower"], n, 0, p);
		result[n] = sp;
	}
	return result;
}

tm inGameTime;
//鼠标
extern LPPOINT lpPoint;
//地图资源位置
string mapSourcePaths[MAP_SOURCE_NUM] = {
	"Sources/Textures/SeasideAndYatai.png",
	"Sources/Textures/Snowfield.png",
	"Sources/Textures/Mountain.png",
	"Sources/Textures/Wood.png",
	"Sources/Textures/Temple_self.png",
	"Sources/Textures/Village.png",
	"6",
	"7",
	"8",
	"9"
};
MAPTILEINF mapSourceInf[MAP_SOURCE_NUM];//地图信息
map<string, vector<SPECIESPRO>> mapSpeciesInf;//地形怪物信息
vector<LPDIRECT3DTEXTURE9> mapSource;//地图图块

SPRITE akashi;
//SPRITE showDamage;
//人物资源
LPDIRECT3DTEXTURE9 imgakashi = NULL;
LPDIRECT3DTEXTURE9 imganpc1 = NULL;
LPDIRECT3DTEXTURE9 imgknight0 = NULL;
LPDIRECT3DTEXTURE9 imgknight1 = NULL;
LPDIRECT3DTEXTURE9 imgknight2 = NULL;
LPDIRECT3DTEXTURE9 imgknight3 = NULL;
LPDIRECT3DTEXTURE9 imglucy = NULL;
LPDIRECT3DTEXTURE9 imgboar = NULL;
LPDIRECT3DTEXTURE9 imgiceCat = NULL;
LPDIRECT3DTEXTURE9 imgslime = NULL;
LPDIRECT3DTEXTURE9 imgprincess = NULL;
//图标
LPDIRECT3DTEXTURE9 imgknight0_icon = NULL;
LPDIRECT3DTEXTURE9 imgglassman_icon = NULL;
LPDIRECT3DTEXTURE9 imgloli_icon = NULL;
LPDIRECT3DTEXTURE9 imgprincess_icon = NULL;
LPDIRECT3DTEXTURE9 imgstartGame_icon = NULL;
//其他资源
LPDIRECT3DTEXTURE9 imgstatusHP = NULL;
LPDIRECT3DTEXTURE9 imgstatusMP = NULL;
LPDIRECT3DTEXTURE9 imgdamageNum = NULL;
LPDIRECT3DTEXTURE9 imgdialogBG = NULL;
LPDIRECT3DTEXTURE9 imgstatusBG = NULL;
LPDIRECT3DTEXTURE9 imgstatusCOVER = NULL;
LPDIRECT3DTEXTURE9 imgblackBG = NULL;
LPDIRECT3DTEXTURE9 imgmainMenu = NULL;

CSound *sound_normal_dead = NULL;
CSound *sound_sword_small = NULL;
CSound *sound_mission_c = NULL;
CSound *sound_icefield_bg = NULL;
CSound *sound_village_bg = NULL;
CSound *sound_seaside_bg = NULL;
CSound *sound_forest_bg = NULL;
CSound *sound_mainmenu_bg = NULL;

//Dialog dialog;

RECT gameworldRect;
LPDIRECT3DSURFACE9 gameworld = NULL;//游戏地图

LPD3DXFONT fontArial36 = NULL;
LPD3DXFONT fontTimesNewRoman40 = NULL;
ParticleClass*	g_pSnowParticles = NULL;    //雪花粒子系统的指针实例
MAP earth_map;
CHARACTOR *player;
DAMAGENUMCOM damageNums;
EventSystem eventsystem;
AudioManager audioManager;
Hermes hermes;
OriginAudio audio;


vector<CHARACTOR> constantNPCs;
vector<CHARACTOR> temporaryNPCs;
vector<DIALOG> newDialogs;
map<string, DIALOG> nonGameDialogs;
map<string, SPRITEINF> spriteInfos;
map<string, SPECIES> SpeciesMap;

//读取地图数据
MAP loadMapFromJson(string filepath)
{
	MAP map;
	//读取文件
	ifstream ReadFile;
	ReadFile.open(filepath);
	if (ReadFile.fail())
	{
		cout << "open json file failed." << endl;
		return map;
	}
	//装载数据
	json j;
	ReadFile >> j;
	json j0 = j["tilesets"][0];
	string s = j0["source"];
	int msi = -1;
	for (int i = 0; i < MAP_SOURCE_NUM; i++)
	{
		if (mapSourcePaths[i] == s)
		{
			msi = i;
			break;
		}
	}
	auto size = j["layers"].size();
	for (int i = 0; i < (int)size; i++)
	{
		//读取每层地图的数据
		json temp = j["layers"][i];
		string name = temp["type"];
		if (name == "tilelayer")
		{
			MAPBLOCK mapblock;
			mapblock.width = temp["width"], mapblock.height = temp["height"];
			mapblock.x = temp["x"], mapblock.y = temp["y"];
			mapblock.mapSourceIndex = msi;
			for (int j = 0; j < mapblock.width*mapblock.height; j++)
			{
				//装填mapindex数据
				int tilenum = temp["data"][j];
				mapblock.mapindex.push_back(MAPINDEX(j, tilenum-1));
			}
			//装填mapblock
			map.maplist.push_back(mapblock);
		}
	}
	return map;
}
//读取地图的可移动信息
bool loadObstacleFromJson(string texpath, vector<short>& obdata,string& terrianType)
{
	string str = texpath.replace(texpath.find(".") + 1, 4, "json");
	//读取文件
	ifstream ReadFile;
	ReadFile.open(str);
	if (ReadFile.fail())
	{
		//输出错误信息
		cout << "open json file failed." << endl;
		return false;
	}
	//装载数据
	json j;
	ReadFile >> j;
	int wid = j["width"];
	int hei = j["height"];
	string terrian = j["terrian"];
	terrianType = terrian;
	vector<short> as(wid*hei, -1);
	for (int t = 0; t < wid*hei; t++)
	{
		as[t] = j["obstacle"][t];
	}
	obdata.swap(as);
	return true;
}
//读取任务信息
vector<MISSION> loadMissionFromJson(string path)
{
	//读取文件
	ifstream ReadFile;
	ReadFile.open(path);
	if (ReadFile.fail())
	{
		//输出错误信息
		cout << "open json file failed." << endl;
		//return false;
	}
	//装载数据
	json j;
	ReadFile >> j;
	auto size = j["missons"].size();
	vector<MISSION> ms;
	MISSION deafultM = MISSION();
	ms.push_back(deafultM);
	for (int i = 0; i < size; i++)
	{
		json tempMission = j["missons"][i];
		int missionId = tempMission["id"];
		string starterName= tempMission["starter"];
		string receiverName = tempMission["receiver"];
		MissionEvent type = MissionEvent(tempMission["missiontype"]);
		string title = tempMission["missionname"];
		string desc = tempMission["missiondesc"];
		vector<MISSIONTARGET> mts;
		//mission target
		auto mtSize = tempMission["missiontarget"].size();
		for (int j = 0; j < mtSize; j++)
		{
			json tempTarget = tempMission["missiontarget"][j];
			string name = tempTarget["targetname"];
			int num = tempTarget["targetnum"];
			MISSIONTARGET mt = MISSIONTARGET(name, num);
			mts.push_back(mt);
		}
		MISSION m = MISSION(missionId, Utf8ToGbk(starterName.c_str()), type, Utf8ToGbk(title.c_str()), Utf8ToGbk(desc.c_str()), mts);
		m.receiver = receiverName;
		//mission talks
		auto tkSize = tempMission["talk"].size();
		for (int k = 0; k < (tkSize < 2 ? tkSize : 2); k++)
		{
			/*
			这里的三元居然需要括起来？！
			经过测试解答：<小于的优先级位于第8组，而:?三元运算符的优先级位于第15组，因此“k < tkSize < 2”视作连续小于进行运算。
			小于符号从左到右结合，false视为0，true视为1((false == 0)==true,true<1==false,true<2==true)参与后边的运算。举例：
			0 < tkSize < 1 == false;
			3 < tkSize < 1 == true;
			而无论true还是false,tkSize和2在运行正常情况下都视作true( >0 ),所以会无限循环
			*/
			json tempTalk = tempMission["talk"][k];
			//未接受
			auto naSize = tempTalk["notaccept"].size();
			for (int nat = 0; nat < naSize; nat++)//NotAcceptTalks
			{
				json tk = tempTalk["notaccept"][nat];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				//m.talks[k]["notaccept"][nat] = t;
				m.talks[k]["notaccept"].push_back(t);//任务.talk[发起者/接收者][状态][话语序号]
			}
			//进行中
			auto uwSize = tempTalk["underway"].size();
			for (int uwt = 0; uwt < uwSize; uwt++)
			{
				json tk = tempTalk["underway"][uwt];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				m.talks[k]["underway"].push_back(t);
			}
			//已完成
			auto clSize = tempTalk["complete"].size();
			for (int clt = 0; clt < clSize; clt++)
			{
				json tk = tempTalk["complete"][clt];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				m.talks[k]["complete"].push_back(t);
			}
			//已结束
			auto fiSize = tempTalk["finish"].size();
			for (int fit = 0; fit < fiSize; fit++)
			{
				json tk = tempTalk["finish"][fit];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				m.talks[k]["finish"].push_back(t);
			}
		}
		
		ms.push_back(m);
	}	
	return ms;
}
//初始化瓦片资源
bool InitMapSource(vector<LPDIRECT3DTEXTURE9> &mapSource,HWND window)
{
	int i = 0;
	//从mapSourcePaths路径装载地图块资源
	for each (string var in mapSourcePaths)
	{
		LPDIRECT3DTEXTURE9 ms = LoadTexture(var);
		//获取每张资源的图片块数
		D3DXIMAGE_INFO info;
		HRESULT result = D3DXGetImageInfoFromFile(var.c_str(), &info);
		//LPDIRECT3DSURFACE9 ms = LoadSurface(var,D3DCOLOR_ARGB(255,0,0,0));
		if (!ms||result != S_OK)
		{
			string errorMessage = "Error load map source in" + var;
			//MessageBox(window,errorMessage.c_str() , "Initiate map resource error", MB_OK | MB_ICONERROR);
			//return false;
		}
		else
		{
			mapSource.push_back(ms);
			mapSourceInf[i].width = info.Width / TILE_PIXEL_NUM;
			mapSourceInf[i].height = info.Height / TILE_PIXEL_NUM;
			mapSourceInf[i].tex = &mapSource[i];
			//顺道读取障碍物和地形类型信息
			loadObstacleFromJson(var, mapSourceInf[i].obstacles, mapSourceInf[i].terrian);
		}
		i++;
	}
	return true;
}

//组建地图
LPDIRECT3DSURFACE9 BuildWorldMap(MAP& map,CHARACTOR player)
{
	int left = ScrollX,up=ScrollY,right=left+SCREENW,down=up+SCREENH;
	RECT screenRect={left,up,right,down};
	gameworldRect = screenRect;
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 100, 200), 1.0f, 0);
	spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
	//对于每一个地图块
	for (int i = 0; i < (int)map.maplist.size(); i++)
	{
		MAPBLOCK& m = map.maplist[i];
		//图块资源编号
		int mapSourceIndex = m.mapSourceIndex;
		//对应位置的RECT
		RECT mapblockRect = { m.x,m.y,m.x + m.width*TILE_PIXEL_NUM,m.y + m.height*TILE_PIXEL_NUM };
		//如果这个mapblock有需要绘制的部分
		if (RECTContain(screenRect, mapblockRect))
		{
			for (int j = 0; j < (int)m.mapindex.size(); j++)
			{
				MAPINDEX& index = m.mapindex[j];
				if (-1 == index.tilenum)
				{
					continue;
				}
				//检查是否要绘制这个瓦片
				long left1 = m.x + (index.number%m.width)*TILE_PIXEL_NUM, top1 = m.y + (index.number / m.width)*TILE_PIXEL_NUM;
				RECT indexRect = { left1,top1,left1 + TILE_PIXEL_NUM,top1 + TILE_PIXEL_NUM };
				if (RECTContain(screenRect, indexRect))
				{
					//绘制到目标表面上
					int left2 = index.tilenum % (mapSourceInf[mapSourceIndex].width)*TILE_PIXEL_NUM, top2 = index.tilenum / (mapSourceInf[mapSourceIndex].height)*TILE_PIXEL_NUM;
					RECT r1 = { left2,top2,left2 + TILE_PIXEL_NUM,top2 + TILE_PIXEL_NUM };
					RECT r2 = { left1 - ScrollX,top1 - ScrollY,left1 - ScrollX + TILE_PIXEL_NUM,top1 - ScrollY + TILE_PIXEL_NUM };
					SpriteTransformDraw(mapSource[mapSourceIndex], left1 - ScrollX, top1 - ScrollY,TILE_PIXEL_NUM,TILE_PIXEL_NUM,index.tilenum, mapSourceInf[mapSourceIndex].width,0.0f,1.0f, D3DCOLOR_XRGB(255, 255, 255));
					//d3ddev->StretchRect(mapSource[mapSourceIndex], &r1, gameworld, &r2, D3DTEXF_LINEAR);
				}
			}
		}
	}
	spriteobj->End();
	return nullptr;
}
//获取玩家所在屏幕方块
RECT GetScreenRect(CHARACTOR player,GameTimer &gt)
{
	RECT result;
	int up, le, down, ri;
	int left = 0, top = 0;
	int plx = (int)player.position[0], ply = (int)player.position[1];
	switch (player.faceDirection)
	{
		//up
	case DIRECTION_UP:
		up = (int)(0.6*SCREENH);
		left = plx - SCREENW / 2, top = ply - up;
		//result = { plx - SCREENW / 2,ply - up,plx + SCREENW / 2,ply - up + SCREENH };
		break;
		//left
	case DIRECTION_LEFT:
		le = (int)(0.6*SCREENW);
		left = plx - le, top = ply - SCREENH / 2;
		//result = { plx - le,ply - SCREENH/2,plx - le+SCREENW,ply + SCREENH/2 };
		break;
		//down
	case DIRECTION_DOWN:
		down = (int)(0.4*SCREENH);
		left = plx - SCREENW / 2, top = ply - down;
		//result = { plx - SCREENW / 2,ply - down,plx + SCREENW / 2,ply - down + SCREENH };
		break;
		//right
	case DIRECTION_RIGHT:
		ri = (int)(0.4*SCREENW);
		left = plx - ri, top = ply - SCREENH / 2;
		//result = { plx - ri,ply - SCREENH / 2,plx - ri + SCREENW,ply + SCREENH / 2 };
		break;
	default:
		left = plx - SCREENW / 2, top = ply - SCREENH / 2;
		//result = { plx - SCREENW / 2,ply - SCREENH / 2,plx + SCREENW / 2,ply + SCREENH / 2 };
		break;
	}
	if (gt.DeltaTime() == -1.0f)//在GameInit()里的调用
	{
		ScrollX = left;
		ScrollY = top;
	}
	else
	{
		int moveSpeed = 50;
		if (left != ScrollX)
		{
			//left = ScrollX > left ? ScrollX - moveSpeed : ScrollX + moveSpeed;
			left = ScrollX > left ? ScrollX - left > moveSpeed ? ScrollX - moveSpeed : left : left - ScrollX > moveSpeed ? ScrollX + moveSpeed : left;

		}
		if (top != ScrollY)
		{
			top = ScrollY > top ? ScrollY - top > moveSpeed ? ScrollY - moveSpeed : top : top - ScrollY > moveSpeed ? ScrollY + moveSpeed : top;
			//top = ScrollY > top ? ScrollY - moveSpeed : ScrollY + moveSpeed;

		}
	}
	
	result = { left,top,left + SCREENW,top + SCREENH };
	if (result.left < 0)
	{
		result.right += 0 - result.left, result.left = 0;
	}
	if (result.right > MaxMapX)
	{
		result.left -= result.right - MaxMapX, result.right = MaxMapX;
	}
	if (result.top < 0)
	{
		result.bottom += 0 - result.top, result.top = 0;
	}
	if (result.bottom > MaxMapY)
	{
		result.top -= result.bottom - MaxMapY, result.bottom = MaxMapY;
	}
	ScrollX = result.left;
	ScrollY = result.top;
	return result;
}
//绘制地图
bool DrawCurrentMap(CHARACTOR player, GameTimer &gt)
{
	//获取当前地图的位置
	RECT screenRect = GetScreenRect(player, gt);
	BuildWorldMap(earth_map, player);
	return true;
}
//获取一个合理的怪物产生位置
VEC getEnemyPos(CHARACTOR& player, GameTimer &mTimer)
{
	//不产怪区（屏幕范围内）
	RECT inner = GetScreenRect(player, mTimer);
	//产怪区
	RECT outer = RECT{ inner.left - EMY_SPN_DIS > 0 ? inner.left - EMY_SPN_DIS : 0,inner.top - EMY_SPN_DIS > 0 ? inner.top - EMY_SPN_DIS : 0,inner.right + EMY_SPN_DIS > 0 ? inner.right + EMY_SPN_DIS : 0,inner.bottom + EMY_SPN_DIS > 0 ? inner.bottom + EMY_SPN_DIS : 0 };
	//随机两个0-1间的数
	float x0 = rand() / double(RAND_MAX);
	float y0 = rand() / double(RAND_MAX);
	//按屏幕大小计算随机位置
	int x = x0 * (outer.right - outer.left);
	int y = y0 * (outer.bottom - outer.top);
	//如果区间在不产怪区
	if (POINTInside(outer.left+x,outer.top+y,inner))
	{
		int i = 0;
		if (x0>0.5f)
		{
			i += 1;
		}
		if (y0>0.5f)
		{
			i += 10;
		}
		/*
		00（2象限）	|  01（1象限）
		————————————————
		10（3象限）	|  11（4象限）


		*/
		//既然有可能一边是0，还能这样算吗？
		switch (i)
		{
		case 0:
			x = x0*EMY_SPN_DIS;
			y = y0*EMY_SPN_DIS;
			break;
		case 1:
			x = EMY_SPN_DIS + SCREENW + x0*EMY_SPN_DIS;
			y = y0*EMY_SPN_DIS;
			break;
		case 10:
			x = x0*EMY_SPN_DIS;
			y = EMY_SPN_DIS + SCREENH + y0*EMY_SPN_DIS;
			break;
		case 11:
			x = EMY_SPN_DIS + SCREENW + x0*EMY_SPN_DIS;
			y = EMY_SPN_DIS + SCREENH + y0*EMY_SPN_DIS;
			break;
		default:
			break;
		}
	}
	return VEC{ (float)(x + outer.left),(float)(y + outer.top) };
}

CHARACTOR* GetPlayer(vector<CHARACTOR>& chara,string tag="player") {
	for (auto i = 0; i < (int)chara.size(); i++)
	{
		if (chara[i].tag == tag)
		{
			return &chara[i];
		}
	}
	return nullptr;
}

string getTerrianFromPosition(int x, int y);
string getSpeciesFromTerrian(int x, int y);
bool saveGame(string path);
bool loadGame(string path);

//集中输入的读取部分
void CheckInput(HWND window,string& text, GameTimer &mTimer) {
	auto nowTime = mTimer.TotalTime();
	
	if (MouseButton(0))
	{
		//设置玩家的行走目标
		GetCursorPos(lpPoint);
		ScreenToClient(window, lpPoint);
		if (gamestate == GameState::menu)
		{
			if (nonGameDialogs.at("menu").buttons[0].click(lpPoint->x, lpPoint->y))
			{
				gamestate = GameState::play_ing;
				DIALOG::LastDialogTime = nowTime;
				AudioLocator::getAudio()->stopSound("mainmenuBGM");
			}
			//else if optionbutton.clicked,gamestate=option
			//else if aboutbutton.clicked,gamestate=about
		}
		else if (gamestate == GameState::play_ing)
		{
			//如果离上次窗口关闭时间小于0.5秒就禁止移动，防止关闭窗口的点击事件漏到下次循环变成移动目标的坐标
			if (nowTime - DIALOG::LastDialogTime > 0.5f)
			{

				int XonMap = lpPoint->x + ScrollX, YonMap = lpPoint->y + ScrollY;

				if (IsPositionMoveable((float)XonMap, (float)YonMap))
				{
					player->target[0] = XonMap, player->target[1] = YonMap;
				}
				else
				{
					//text = "Left Trigger";
					text += " NOT OK";
				}
			}
		}
		else if (gamestate == GameState::play_dialog)
		{
			if ((int)newDialogs.size() > 0)
			{
				//获取最底层Dialog
				DIALOG& d = newDialogs[(int)newDialogs.size() - 1];
				if (d.isClicked(lpPoint->x, lpPoint->y) && (nowTime - DIALOG::LastDialogTime > 0.5f))
				{
					newDialogs.erase(newDialogs.end() - 1, newDialogs.end());
					//把最后一次关闭所有窗口的时间记下来
					DIALOG::LastDialogTime = nowTime;
				}
			}
		}
		else if (gamestate == GameState::play_pause)
		{

		}
		//auto nowTime = mTimer.TotalTime();
		
	}
	if (KeyDown(DIK_W))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			player->isMannualMoving = true;
			player->faceDirection = DIRECTION_UP;
			//player.Move(DIRECTION_UP, mTimer.DeltaTime());
			//text = "W";
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_S))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			player->isMannualMoving = true;
			player->faceDirection = DIRECTION_DOWN;
			//player.Move(DIRECTION_DOWN, mTimer.DeltaTime());
			//text = "S";
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_A))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			player->isMannualMoving = true;
			player->faceDirection = DIRECTION_LEFT;
			//player.Move(DIRECTION_LEFT, mTimer.DeltaTime());
			//text = "A";
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_D))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			player->isMannualMoving = true;
			player->faceDirection = DIRECTION_RIGHT;
			//player.Move(DIRECTION_RIGHT, mTimer.DeltaTime());
			//text = "D";
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_F))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			//damageNums.addDamageNum(114514, 500, 500);
			//AudioLocator::getAudio()->playSound("NormalDead");
			//const CHARACTOR* test= CharaLocator::getChara("knight");
			//time_t now = time(0);
			//tm *lotm = localtime(&now);
			//getSceneColorFromTime(lotm);
			player->position[0] = 2500, player->position[1] = 1000;
			//text = getTerrianFromPosition(player->position[0], player->position[1]);
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_G))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			player->Damage(1);
			
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_H))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			if (player->HP < player->MaxHP&&player->MP>0)
			{
				player->Damage(1, 1);
				player->Heal(1);
			}	
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_J))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			CHARACTOR*p = player->GetNearbyChara();
			if (p != NULL)
			{
				player->Attack(*p);
				//方向判定
				float distanceX = (player->position[0] - p->position[0]), distanceY = (player->position[1] - p->position[1]);
				if (abs(distanceX) >= abs(distanceY))//左或右，在线上则视为左右
				{
					p->faceDirection = distanceX > 0 ? 3 : 1;//按X正负区分左右
				}
				else
				{
					p->faceDirection = distanceY > 0 ? 2 : 0;//按Y区分上下
				}
				//读取当前的显示帧范围
				int dir = p->faceDirection;
				int index = p->animeIndex[dir][0];
				int min = p->animeIndex[index][1];
				int max = p->animeIndex[index][1] + p->animeIndex[index][2] - 1;
				if (p->frame > max || p->frame < min)
				{
					p->frame = min;
					p->animateTick = 0.0f;
				}
				if (p->constant != true)
				{
					p->Attack(*player);
				}
			}
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_E))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			CHARACTOR*p = player->GetNearbyChara();
			if (p != NULL&&p->constant==true)
			{
				//方向判定
				float distanceX = (player->position[0] - p->position[0]), distanceY = (player->position[1] - p->position[1]);
				if (abs(distanceX) >= abs(distanceY))//左或右，在线上则视为左右
				{
					p->faceDirection = distanceX > 0 ? 3 : 1;//按X正负区分左右
				}
				else
				{
					p->faceDirection = distanceY > 0 ? 2 : 0;//按Y区分上下
				}
				//读取当前的显示帧范围
				int dir = p->faceDirection;
				int index = p->animeIndex[dir][0];
				int min = p->animeIndex[index][1];
				int max = p->animeIndex[index][1] + p->animeIndex[index][2] - 1;
				if (p->frame > max || p->frame < min)
				{
					p->frame = min;
					p->animateTick = 0.0f;
				}

				MISSION& m = eventsystem.getMissionByName(p->tag);
				if (m.ID != -1)
				{
					if (p->tag == m.starter)
					{
						//未接受状态
						if (m.eventState == MissionState::NotAccept)
						{
							//提取Dialog
							for (auto var = m.talks[0].at("notaccept").rbegin(); var != m.talks[0].at("notaccept").rend(); var++)
							{
								if (var->CharaName == "Backgound")
								{
									CharaLocator::getChara(var->CharaName);
								}
								else if (var->CharaName == "player")
								{
									newDialogs.push_back(DIALOG(DialogType::messageBox, *player, var->message));
								}
								else
								{
									const CHARACTOR* c = CharaLocator::getChara(var->CharaName);
									newDialogs.push_back(DIALOG(DialogType::messageBox, *c, var->message));
								}
							}
							//改变任务状态
							m.eventState = MissionState::Underway;
						}
						//进行中
						else if (m.eventState == MissionState::Underway)
						{
							//提取Dialog
							for (auto var = m.talks[0].at("underway").rbegin(); var != m.talks[0].at("underway").rend(); var++)
							{
								if (var->CharaName == "Backgound")
								{
									CharaLocator::getChara(var->CharaName);
								}
								else if (var->CharaName == "player")
								{
									newDialogs.push_back(DIALOG(DialogType::messageBox, *player, var->message));
								}
								else
								{
									const CHARACTOR* c = CharaLocator::getChara(var->CharaName);
									newDialogs.push_back(DIALOG(DialogType::messageBox, *c, var->message));
								}
							}
						}
						//已完成
						else if (m.eventState == MissionState::Complete)
						{
							//提取Dialog
							for (auto var = m.talks[0].at("complete").rbegin(); var != m.talks[0].at("complete").rend(); var++)
							{
								if (var->CharaName == "Backgound")
								{
									CharaLocator::getChara(var->CharaName);
								}
								else if (var->CharaName == "player")
								{
									newDialogs.push_back(DIALOG(DialogType::messageBox, *player, var->message));
								}
								else
								{
									const CHARACTOR* c = CharaLocator::getChara(var->CharaName);
									newDialogs.push_back(DIALOG(DialogType::messageBox, *c, var->message));
								}
							}
							m.eventState = MissionState::Finish;
							if (m.ID = 2)
							{
								MISSION& nm = eventsystem.getMissionByID(3);
								nm.eventState = MissionState::Complete;
							}
						}
					}
					else// if(p->tag == m.receiver)
					{
						if (m.eventState == MissionState::NotAccept)
						{
							//提取Dialog
							for (auto var = m.talks[1].at("notaccept").rbegin(); var != m.talks[1].at("notaccept").rend(); var++)
							{
								if (var->CharaName == "Backgound")
								{
									CharaLocator::getChara(var->CharaName);
								}
								else if (var->CharaName == "player")
								{
									newDialogs.push_back(DIALOG(DialogType::messageBox, *player, var->message));
								}
								else
								{
									const CHARACTOR* c = CharaLocator::getChara(var->CharaName);
									newDialogs.push_back(DIALOG(DialogType::messageBox, *c, var->message));
								}
							}
						}
						else if (m.eventState == MissionState::Underway)
						{
							//提取Dialog
							for (auto var = m.talks[1].at("underway").rbegin(); var != m.talks[1].at("underway").rend(); var++)
							{
								if (var->CharaName == "Backgound")
								{
									CharaLocator::getChara(var->CharaName);
								}
								else if (var->CharaName == "player")
								{
									newDialogs.push_back(DIALOG(DialogType::messageBox, *player, var->message));
								}
								else
								{
									const CHARACTOR* c = CharaLocator::getChara(var->CharaName);
									newDialogs.push_back(DIALOG(DialogType::messageBox, *c, var->message));
								}
							}
						}
						//已完成
						else if (m.eventState == MissionState::Complete)
						{
							//提取Dialog
							for (auto var = m.talks[1].at("complete").rbegin(); var != m.talks[1].at("complete").rend(); var++)
							{
								if (var->CharaName == "Backgound")
								{
									CharaLocator::getChara(var->CharaName);
								}
								else if (var->CharaName == "player")
								{
									newDialogs.push_back(DIALOG(DialogType::messageBox, *player, var->message));
								}
								else
								{
									const CHARACTOR* c = CharaLocator::getChara(var->CharaName);
									newDialogs.push_back(DIALOG(DialogType::messageBox, *c, var->message));
								}
							}
							m.eventState = MissionState::Finish;
							if (m.ID = 3)
							{
								MISSION& nm = eventsystem.getMissionByID(4);
								if (nm.ID != -1)
								{
									nm.eventState = MissionState::Complete;
									player->position[0] = 10300, player->position[1] = 1700;
								}

							}
						}
					}
				}
				else
				{
					string s = "我的名字是" + p->tag + "，有什么事吗？";
					if (p->tag == "knight")
					{
						s = "感谢您，我现在不用怕被长官骂了。";
					}
					else if (p->tag == "manwithglass")
					{
						s = "还想过河吗？抱歉需要重新买票。";
					}
					
					//string s = "小姐，我手下的船需要买船票……你有啊！那没事了。";
					newDialogs.push_back(DIALOG(DialogType::messageBox, *p, s));
				}
			}
			else
			{

			}
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (KeyDown(DIK_I))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{
			for (int i = 0; i < (int)constantNPCs.size(); i++)
			{
				if (player == &constantNPCs[i])
				{
					if (i - 1 < 0)
					{
						player = &constantNPCs[constantNPCs.size() - 1];
					}
					else
					{
						player = &constantNPCs[i - 1];
					}
					break;
				}
			}
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}

	if (MouseButton(1))
	{
		if (gamestate == GameState::menu)
		{

		}
		else if (gamestate == GameState::play_ing)
		{

		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
	}
	if (MouseButton(2))
	{
		text = "Mouse Scroll";
	}
	if (MouseWheel())
	{
		text = to_string(MouseWheel());
	}

	if (KeyDown(DIK_ESCAPE))
	{
		if(nowTime - DIALOG::LastDialogTime > 0.5f)
		if (gamestate == GameState::menu)
		{
			DIALOG::LastDialogTime = nowTime;
			gameover = true;
		}
		else if (gamestate == GameState::play_ing)
		{
			DIALOG::LastDialogTime = nowTime;
			saveGame("Save.json");
			player->currTerrian = "null";
			hermes.notify(*player, EVENT_ATLOCATION);
			player->lastTerrian = "null";
			AudioLocator::getAudio()->loopSound("mainmenuBGM");
			gamestate = GameState::menu;
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
		
	}
	if (controllers[0].wButtons & XINPUT_GAMEPAD_BACK)
	{
		if (gamestate == GameState::menu)
		{
			gameover = true;
		}
		else if (gamestate == GameState::play_ing)
		{
			gamestate = GameState::menu;
		}
		else if (gamestate == GameState::play_dialog)
		{

		}
		else if (gamestate == GameState::play_pause)
		{

		}
		else if (gamestate == GameState::option)
		{

		}
	}
}

void UpdateDamageNums(vector<DAMAGENUM> &damageNums, GameTimer &mTimer)
{
	for (auto iter = damageNums.begin(); iter != damageNums.end();)
	{
		if ((int)iter->numbers.size()==0)
		{
			iter = damageNums.erase(iter);
			if (iter == damageNums.end())
				break;
		}
		else
		{
			iter->update(mTimer.DeltaTime());
			iter++;
		}
	}
}

LPDIRECT3DTEXTURE9 LoadTextureSafe(HWND window, string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0))
{
	LPDIRECT3DTEXTURE9 texture = LoadTexture(filename, transcolor);
	if (!texture)
	{
		string infor = "Error load image in " + filename;
		MessageBox(window,infor.c_str() , "Error!", MB_OK | MB_ICONERROR);
	}
	return texture;
}

void spawnNPCs()
{

	CHARACTOR p, npc0, npc1, npc2;

	p = CHARACTOR(SpeciesMap.at("约尔顿人"), spriteInfos.at("merchant"), 1, D3DVECTOR{ 9250,2600,0 }, "manwithglass", true);
	p.icon = imgglassman_icon;
	constantNPCs.push_back(p);

	npc0 = CHARACTOR(SpeciesMap.at("安东尼人"), spriteInfos.at("knight"), 1, D3DVECTOR{ 3750,1610,0 }, "knight", true);
	npc0.icon = imgknight0_icon;
	constantNPCs.push_back(npc0);

	npc1 = CHARACTOR(SpeciesMap.at("约尔顿人"), spriteInfos.at("loli"), 1, D3DVECTOR{ 2350,2650,0 }, "loli", true);
	npc1.icon = imgloli_icon;
	constantNPCs.push_back(npc1);

	npc2 = CHARACTOR(SpeciesMap.at("安东尼人"), spriteInfos.at("princess"), 1, D3DVECTOR{ 12450,1150,0 }, "princess", true);
	npc2.icon = imgprincess_icon;
	constantNPCs.push_back(npc2);
}
//游戏初始化
bool GameInit(HWND window)
{
	//加载Direct3D组件(这里假设了屏幕分辨率为1080P)
	if (!Direct3DInit(window, 1920, 1080, false))
	{
		MessageBox(window, "Error initializing DirectX3D", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!DirectInputInit(window))
	{
		MessageBox(window, "Error initializing DirectInput", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	//initialize DirectSound
	if (!DirectSoundInit(window))
	{
		MessageBox(window, "Error initializing DirectSound", APPTITLE.c_str(), 0);
		return false;
	}
	//服务定位器设置定位指针
	AudioLocator::setService(&audio);
	CharaLocator::setService(&constantNPCs);
	MissionSystemLocator::setService(&eventsystem);
	//装载图像
	imgakashi = LoadTexture("Sources/Textures/akashi.png");
	if (!imgakashi)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	imganpc1 = LoadTexture("Sources/Textures/professor.png");
	if (!imganpc1)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	imgknight0= LoadTexture("Sources/Textures/knight1.png", D3DCOLOR_XRGB(255, 255, 255));
	if (!imgknight0)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	imglucy = LoadTexture("Sources/Textures/lucy-heartfilia1.png", D3DCOLOR_XRGB(255, 255, 255));
	if (!imglucy)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	imgboar = LoadTexture("Sources/Textures/boar.png", D3DCOLOR_XRGB(255, 255, 255));
	if (!imgboar)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	imgknight0_icon= LoadTexture("Sources/Textures/Icons/s2.png", D3DCOLOR_XRGB(255, 255, 255));
	if (!imgknight0_icon)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	imgdialogBG = LoadTexture("Sources/Textures/Commonframe_3.tga", D3DCOLOR_XRGB(255, 0, 0));
	if (!imgdialogBG)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	DIALOG::msgBoxBackGround = imgdialogBG;
	imgstatusBG = LoadTextureSafe(window, "Sources/Textures/statusBG.png", D3DCOLOR_XRGB(255, 255, 255));
	DIALOG::statusBoxBackGround = imgstatusBG;
	imgstatusCOVER = LoadTextureSafe(window, "Sources/Textures/statusCover.png", D3DCOLOR_XRGB(255, 255, 255));
	DIALOG::statusBoxCover = imgstatusCOVER;
	imgdamageNum = LoadTexture("Sources/Textures/DamageNumbers.png", D3DCOLOR_XRGB(255, 255, 255));
	if (!imgdamageNum)
	{
		MessageBox(window, "Error load image", "Error!", MB_OK | MB_ICONERROR);
		return false;
	}
	damageNums.getNumTexture("Sources/Textures/DamageNumbers.png");
	imgiceCat = LoadTextureSafe(window, "Sources/Textures/sprites/IceCat.png");
	imgslime = LoadTextureSafe(window, "Sources/Textures/sprites/Slime.png");
	imgprincess = LoadTextureSafe(window, "Sources/Textures/sprites/Princess.png");
	imgglassman_icon = LoadTextureSafe(window, "Sources/Textures/Icons/gr1.png");
	imgloli_icon = LoadTextureSafe(window, "Sources/Textures/Icons/c5.png");
	imgprincess_icon = LoadTextureSafe(window, "Sources/Textures/Icons/f2.png");
	imgblackBG = LoadTextureSafe(window, "Sources/Textures/BlackBackground.png");
	DIALOG::fullscreenBlackBackGround = imgblackBG;
	imgstatusHP = LoadTextureSafe(window, "Sources/Textures/statusHP.png");
	DIALOG::statusHP = imgstatusHP;
	imgstatusMP = LoadTextureSafe(window, "Sources/Textures/statusMP.png");
	DIALOG::statusMP = imgstatusMP;
	imgmainMenu = LoadTextureSafe(window, "Sources/Textures/mainMenu.jpg");
	imgstartGame_icon = LoadTextureSafe(window, "Sources/Textures/startButton.png");
	//加载音频
	sound_normal_dead = LoadSound("Sources/Sounds/monsterDeath02.wav");
	if (!sound_normal_dead)
	{
		MessageBox(window, "Error loading step.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("NormalDead", sound_normal_dead);
	}
	sound_sword_small = LoadSound("Sources/Sounds/Socapex-Swordsmall.wav");
	if(!sound_sword_small)
	{
		MessageBox(window, "Error loading Swordsmall.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("SwordSmall", sound_sword_small);
	}
	sound_mission_c = LoadSound("Sources/Sounds/Success_aigei.wav");
	if (!sound_mission_c)
	{
		MessageBox(window, "Error loading Success_aigei.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("missionComplete", sound_mission_c);
	}
	
	sound_icefield_bg = LoadSound("Sources/Sounds/氷と雪の世界.wav");
	if (!sound_icefield_bg)
	{
		MessageBox(window, "Error loading 氷と雪の世界.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("icefieldBGM", sound_icefield_bg);
	}
	sound_village_bg = LoadSound("Sources/Sounds/旅の始まり.wav");
	if (!sound_village_bg)
	{
		MessageBox(window, "Error loading 旅の始まり.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("villageBGM", sound_village_bg);
	}
	sound_seaside_bg = LoadSound("Sources/Sounds/爽やかな風.wav");
	if (!sound_seaside_bg)
	{
		MessageBox(window, "Error loading 爽やかな風.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("seasideBGM", sound_seaside_bg);
	}
	sound_forest_bg = LoadSound("Sources/Sounds/森を駆ける風.wav");
	if (!sound_forest_bg)
	{
		MessageBox(window, "Error loading 森を駆ける風.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("forestBGM", sound_forest_bg);
	}
	sound_mainmenu_bg = sound_icefield_bg;
	if (!sound_mainmenu_bg)
	{
		MessageBox(window, "Error loading 共闘.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("mainmenuBGM", sound_mainmenu_bg);
	}
	//给游戏系统的代理人加两个观察者
	hermes.addObserver(&eventsystem);
	hermes.addObserver(&audioManager);

	//加载字体
	fontArial36 = MakeFont("Arial", 36);
	fontTimesNewRoman40 = MakeFont("TimesNewRoman", 40);
	//加载游戏数据
	SpeciesMap = loadSpeciesMapFronJson("Sources/GameData.json");
	eventsystem.setMissions(loadMissionFromJson("Sources/missions.json"));
	//加载精灵
	akashi = GetSpriteSize("Sources/Textures/akashi.png", &akashi, 8, 6);
	akashi.x = 0, akashi.y = 0;
	akashi.startframe = 0, akashi.endframe = 47;
	akashi.delay = 60;

	std::map<std::string, DIASTR> menu_str;
	std::map<std::string, DIATEX> menu_tex;
	menu_tex["backGround"] = DIATEX(imgmainMenu, 0, 0, SCREENW, SCREENH);
	std::vector<Button> menu_btn;
	Button start = Button(((SCREENW - 200) / 2), ((SCREENH - 90) / 2), 200, 90,imgstartGame_icon);
	menu_btn.push_back(start);
	DIALOG d_menu = DIALOG(DialogType::fullScreenBox, menu_str, menu_tex, menu_btn);

	nonGameDialogs["menu"] = d_menu;

	//加载角色
	SPRITEINF p0 = SPRITEINF(imganpc1, 9, 1.0f, 0, 35, 66, 0, 0.85f, imgglassman_icon);
	p0.name = "merchant";
	SPRITEINF n0 = SPRITEINF(imgknight0, 4, 0.7f, 0, 15, 99, 0, 0.85f, imgknight0_icon);
	n0.name = "knight";
	SPRITEINF n1 = SPRITEINF(imglucy, 4, 0.15f, 0, 15, 99, 0, 0.85f, imgloli_icon);
	n1.name = "loli";
	SPRITEINF n2 = SPRITEINF(imgprincess, 4, 1.1f, 0, 15, 99, 1, 0.85f, imgprincess_icon);
	n2.name = "princess";
	SPRITEINF b1 = SPRITEINF(imgboar, 4, 1.0f, 0, 15, 99);
	b1.name = "boar";
	spriteInfos["merchant"] = p0;
	spriteInfos["knight"] = n0;
	spriteInfos["loli"] = n1;
	spriteInfos["princess"] = n2;
	spriteInfos["boar"] = b1;

	loadGame("save.json");
	
	//临时timer对象，给GetScreenRect函数用
	GameTimer gt;
	gt.Reset();
	gt.Start();
	//装载地图资源
	InitMapSource(mapSource, window);
	//编辑地图类型的怪物
	SPECIESPRO s=SPECIESPRO("boar",1.0f);
	vector<SPECIESPRO> vs;
	vs.push_back(s);
	mapSpeciesInf["Snowfield"] = vs;
	//装载地图
	earth_map += loadMapFromJson("village.json");
	earth_map += loadMapFromJson("test1.json");
	earth_map += loadMapFromJson("test3.json");
	//earth_map += loadMapFromJson("test4.json");
	earth_map += loadMapFromJson("seaside_right.json");
	earth_map += loadMapFromJson("wood.json");
	earth_map += loadMapFromJson("temple.json");

	//获取maxMapX，maxMapY
	for each (MAPBLOCK var in earth_map.maplist)
	{
		int x = var.x + var.width * 32;
		MaxMapX = x > MaxMapX ? x : MaxMapX;
		int y = var.y + var.height * 32;
		MaxMapY = y > MaxMapY ? y : MaxMapY;
	}
	//如果扩充vector超过了capcity，自动扩容会把player变成野指针
	player = GetPlayer(constantNPCs, "loli");
	//获取屏幕
	GetScreenRect(*player, gt);

	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	//创建并初始化雪花粒子系统
	g_pSnowParticles = new ParticleClass(d3ddev);
	g_pSnowParticles->InitParticle(window);
	gamestate = GameState::menu;
	
	AudioLocator::getAudio()->loopSound("mainmenuBGM");

	return true;
}
//游戏运行
void GameRun(HWND window,GameTimer &mTimer)
{
	if (!d3ddev)
		return;
	DirectInputUpdate();
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET || D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);
	
	//debug信息
	string text = "";
	
	//按键处理
	CheckInput(window,text,mTimer);
	//逻辑处理
	switch (gamestate)
	{
	case GameState::menu:
		//if gamebutton.clicked,gamestate=playing
		break;
	case GameState::play_ing:
		if ((int)newDialogs.size() == 0)
		{
			DIALOG status = DIALOG(DialogType::charaStatus, *player);
			newDialogs.push_back(status);
		}
		if (!gameStart)
		{
			gameStart = true;
			hermes.notify(*player, MissionEvent::EVENT_INGAME);
		}

		for (auto var = newDialogs.begin(); var < newDialogs.end(); var++)
		{
			if (var->type == DialogType::charaState || var->type == DialogType::messageBox || var->type == DialogType::worldmap || var->type == DialogType::hintBox)
			{
				gamestate = GameState::play_dialog;
			}
		}
		break;
	case GameState::play_dialog:
		gamestate = play_ing;
		for (auto var : newDialogs)
		{
			if (var.type == DialogType::charaState || var.type == DialogType::messageBox || var.type == DialogType::worldmap || var.type == DialogType::hintBox)
			{
				gamestate = GameState::play_dialog;
			}
		}
		break;
	case GameState::play_pause:
		break;
	case GameState::option:
		break;
	default:
		break;
	}
	//更新处理
	if (gamestate == GameState::menu)
	{
		nonGameDialogs.at("menu").update(CHARACTOR());
	}
	else if (gamestate == GameState::play_ing || GameState::play_dialog)
	{
		//spawn野怪
		if ((wildEnemyNum<EMY_SPN_NUM) && ((wildSpawnCD -= mTimer.DeltaTime()) <= 0.0))
		{
			wildSpawnCD = EMY_SPN_CD;
			string playerTag = player->tag;
			//SPRITEINF b1 = SPRITEINF(imgboar, 4, 1.0f, 0, 15, 99);
			VEC pos = getEnemyPos(*player, mTimer);
			string species = getSpeciesFromTerrian((int)pos.x, (int)pos.y);
			if (species == "boar")
			{
				CHARACTOR beast0 = CHARACTOR(SpeciesMap.at("野猪"), spriteInfos.at("boar"), 1, D3DVECTOR{ pos.x,pos.y,0 }, "boar");
				constantNPCs.push_back(beast0);
				player = GetPlayer(constantNPCs, playerTag);
				wildEnemyNum++;
			}
		}
		//清除死亡的非玩家
		for (auto iter = constantNPCs.begin(); iter != constantNPCs.end();)
		{
			//利用迭代器重载了解引用运算符*的特点 来源：https://blog.csdn.net/hl_zzl/article/details/84575713
			if (iter->HP <= 0 && &*iter != player)
			{
				iter = constantNPCs.erase(iter);
				wildEnemyNum--;
				if (iter == constantNPCs.end())
					break;
			}
			else
			{
				iter++;
			}
		}
		
		damageNums.update(mTimer.DeltaTime());
		
		//处理精灵的变化
		//SpriteAnimate(akashi.frame, akashi.startframe, akashi.endframe, akashi.direction, akashi.starttime, akashi.delay);
		akashi.x = ScrollX, akashi.y = ScrollY;
		//更新角色状态
		for (int i = 0; i < (int)constantNPCs.size(); i++)
		{
			constantNPCs[i].CharaAnimate(mTimer.DeltaTime());
		}
		for (auto iterDialog = newDialogs.begin(); iterDialog != newDialogs.end(); iterDialog++)
		{
			iterDialog->update(*player);
		}
		string playerTerrian = getTerrianFromPosition(player->position[0], player->position[1]);
		if (playerTerrian != player->lastTerrian)
		{
			player->currTerrian = playerTerrian;
			hermes.notify(*player, EVENT_ATLOCATION);
			player->lastTerrian = playerTerrian;
		}
		if (playerTerrian == "Snowfield")
		{
			snowShow = true;
			g_pSnowParticles->UpdateParticle(mTimer.DeltaTime());
		}
		else
		{
			snowShow = false;
		}
	}
	//debug
	//text = "杀猪计数为" + to_string(eventsystem.getKillNum());
	//text = to_string(player->MPrecoverCD)+","+ to_string(player->MP);
	
	//绘制处理
	if (d3ddev->BeginScene())
	{
		switch (gamestate)
		{
		case GameState::menu:
			spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
			//开始绘制
			nonGameDialogs.at("menu").show();
			if (text != "")
				FontPrint(fontTimesNewRoman40, 0, 200, text, D3DCOLOR_XRGB(0, 0, 100));
			//结束绘制
			spriteobj->End();
			break;
		case GameState::play_ing:
		case GameState::play_dialog:
		case GameState::play_pause:
			DrawCurrentMap(*player, mTimer);
			spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
			//开始绘制

			//绘制雪花粒子系统
			if (snowShow)
			{
				g_pSnowParticles->RenderParticle();
			}
			//SpriteTransformDraw(imgakashi, akashi.x-ScrollX, akashi.y-ScrollY, akashi.width, akashi.height, akashi.frame, akashi.columns);

			DrawCharacters(constantNPCs);
			//SpriteTransformDraw(*player.sprite, player.x+(int)player.position[0]-ScrollX, player.y+(int)player.position[1]-ScrollY, player.width, player.height, player.frame, player.columns);
			if (text != "")
				FontPrint(fontTimesNewRoman40, 0, 200, text, D3DCOLOR_XRGB(0, 0, 100));
			for (auto nums : damageNums.damageObjects)
			{
				nums.draw();
			}
			for (auto d : newDialogs)
			{
				d.show();
			}
			//结束绘制
			spriteobj->End();
			break;
		default:
			break;
		}
		
		
		
		RECT r ={ 0,0,SCREENW,SCREENH };
		d3ddev->EndScene();
		d3ddev->Present(&r, NULL, NULL, NULL);
	}
}
//游戏结束
void GameEnd()
{
	SafeRelease(imgakashi);
	//释放Direct组件
	DirectSoundShutdown();
	DirectInputShutdown();
	Direct3DShutdown();
}

bool saveGame(string path)
{
	
	json j;
	//角色
	int count = 0;
	for (auto iter = constantNPCs.begin(); iter < constantNPCs.end(); iter++)
	{
		if (iter->constant == true)
		{
			string name = GbkToUtf8(iter->tag.c_str());
			string species = GbkToUtf8(iter->race.c_str());
			string spriteinf = GbkToUtf8(iter->profession.c_str());
			json chara = {
				{ "level", iter->level },
				{ "name",name },
				{ "position",{ iter->position[0], iter->position[1], iter->position[2] } },
				{ "species", species },
				{ "spriteinf", spriteinf },
			};
			j["character"][count++] = chara;
		}
		else
		{
			continue;
		}
	}
	//任务
	count = 0;
	EventSystem* missionSys = MissionSystemLocator::getMissionSystem();
	int missionNum = missionSys->Size();
	for (int k = 0; k < missionNum; k++)
	{
		MISSION& m = MissionSystemLocator::getMissionSystem()->getMissionByID(k);
		if (m.ID != -1)
		{
			int i = (int)m.eventState;
			j["missionstate"][count++] = i;
		}
		else
		{
			continue;
		}
	}
	// write prettified JSON to another file
	ofstream o(path);
	if (o.fail())
	{
		cout << "Unable to overwrite save files" << endl;
		return false;
	}
	else
	{
		o << setw(4) << j << std::endl;
		return true;
	}
}

bool loadGame(string path)
{
	std::ifstream i(path);
	if (i.fail())
	{
		spawnNPCs();
		return false;
	}
	else
	{
		json j;
		i >> j;
		int charaSize = j["character"].size();
		for (int k = 0; k < charaSize; k++)
		{
			json jchara = j["character"][k];
			string spe = jchara["species"];
			string species = Utf8ToGbk(spe.c_str());
			string spr = jchara["spriteinf"];
			string spriteinf = Utf8ToGbk(spr.c_str());
			string nam = jchara["name"];
			string name = Utf8ToGbk(nam.c_str());
			SPECIES& s = SpeciesMap.at(species);
			SPRITEINF info = spriteInfos.at(spriteinf);
			int lv = jchara["level"];
			D3DVECTOR v = D3DVECTOR{ jchara["position"][0],jchara["position"][1],jchara["position"][2] };
			CHARACTOR chara = CHARACTOR(s, info, lv, v, name, true);
			constantNPCs.push_back(chara);
		}
		cout << j["missionstate"];
		int missionSaveNum = (int)j["missionstate"].size();
		int missionSysNum = MissionSystemLocator::getMissionSystem()->Size();
		int missionNum = missionSaveNum < missionSysNum ? missionSaveNum : missionSysNum;
		for (int l = 0; l < missionNum; l++)
		{
			MISSION& m = MissionSystemLocator::getMissionSystem()->getMissionByID(l);
			int state = j["missionstate"][l];
			m.eventState = MissionState(state);
		}
		return true;
	}
}

void Mission0action()
{
	MISSION var = eventsystem.getMissionByID(0);
	//提取Dialog
	for (auto i = var.talks[0].at("notaccept").rbegin(); i != var.talks[0].at("notaccept").rend(); i++)
	{
		if (i->CharaName == "Backgound")
		{
			newDialogs.push_back(DIALOG(DialogType::fullScreenBox, *player, i->message));
		}
		else if (i->CharaName == "player")
		{
			newDialogs.push_back(DIALOG(DialogType::messageBox, *player, i->message));
		}
		else
		{
			const CHARACTOR* c = CharaLocator::getChara(i->CharaName);
			newDialogs.push_back(DIALOG(DialogType::messageBox, *c, i->message));
		}
	}

}

void Mission1action()
{
	MISSION var = eventsystem.getMissionByID(1);
	//提取Dialog
	for (auto i = var.talks[0].at("underway").rbegin(); i != var.talks[0].at("underway").rend(); i++)
	{
		if (i->CharaName == "Backgound")
		{
			newDialogs.push_back(DIALOG(DialogType::fullScreenBox, *player, i->message));
		}
		else if (i->CharaName == "player")
		{
			newDialogs.push_back(DIALOG(DialogType::messageBox, *player, i->message));
		}
		else
		{
			const CHARACTOR* c = CharaLocator::getChara(i->CharaName);
			newDialogs.push_back(DIALOG(DialogType::messageBox, *c, i->message));
		}
	}
}

bool Button::click(int mx, int my)
{
	RECT r = { x,y,x + width,y + height };
	return POINTInside(mx, my, r);
}

bool IsPositionMoveable(float x,float y,CHARACTOR* chara)
{
	bool result=true;
	if (x<0 || x>MaxMapX || y<0 || y>MaxMapY)
	{
		return result = false;
	}
	//点击的绝对坐标
	int absX = x, absY = y ;
	//检查被点击到的格子
	for each (MAPBLOCK var in earth_map.maplist)
	{
		RECT rect= { var.x,var.y,var.x + var.width*TILE_PIXEL_NUM,var.y + var.height*TILE_PIXEL_NUM };
		if (absX > rect.left&&absX<rect.right&&absY>rect.top&&absY < rect.bottom)
		{
			//在屏幕中方块的几行几列
			int col = (absX-var.x) / TILE_PIXEL_NUM, row = (absY-var.y) / TILE_PIXEL_NUM;
			int tileNum = var.mapindex[row*var.width+col].tilenum;
			//如果该点没有定义（不在这个mapblock）
			if (-1 == tileNum)
			{
				continue;
			}
			//如果该点是不可通行的
			else if(1 == mapSourceInf[var.mapSourceIndex].obstacles[tileNum])
			{
				result = false;
			}
			//如果该点是超通行块（例：桥使得底层是海洋、河流也能通行）
			else if (2 == mapSourceInf[var.mapSourceIndex].obstacles[tileNum])
			{
				result = true;
				break;
			}
			//如果可通行（=0），则不做处置（要由是否有其他两种情况决定）
		}
	}
	return result;
}

string getTerrianFromPosition(int x, int y)
{
	string result = "";
	for each (MAPBLOCK var in earth_map.maplist)
	{
		RECT rect = var.getRECT();
		if (x > rect.left&&x<rect.right&&y>rect.top&&y < rect.bottom)
		{
			//在屏幕中方块的几行几列
			int col = (x - var.x) / TILE_PIXEL_NUM, row = (y - var.y) / TILE_PIXEL_NUM;
			int tileNum = var.mapindex[row*var.width + col].tilenum;
			//如果没有定义
			if (-1 == tileNum)
			{
				continue;
			}
			//获取资源
			else
			{
				result = mapSourceInf[var.mapSourceIndex].terrian;
				break;
			}
		}
	}
	return result;
}

string getSpeciesFromTerrian(int x, int y)
{
	string result = getTerrianFromPosition(x, y);
	string spResult = "null";
	int num = (int)mapSpeciesInf[result].size();
	if (num > 0)
	{
		vector<SPECIESPRO>& a = mapSpeciesInf[result];
		//总共的几率
		float i = 0.0f;
		for each (auto var in a)
		{
			i += var.probability;
		}
		float scaleCon = 1.0f / i;
		//几率列      *随机数落在的位置
		//0（物种1）/0.25（物种2）/0.75（物种3）/1
		vector<int> species(a.size(), 0);
		int probab = 0;
		for (auto i = 0; i < (int)species.size(); i++)
		{
			probab += scaleCon*a[i].probability * 1000;
			species[i] = probab;
		}
		*species.rbegin() = 1000;//最后的几率为100%

		int random = rand() % 1000;
		for (auto j = 0; j < (int)species.size(); j++)
		{
			if (random < species[j])
			{
				spResult = a[j].speciesName;
			}
		}
		return spResult;
	}
	else
	{
		return spResult;
	}
}

void FindNearestPos()
{

}
//组建动画帧的索引，这里假定了素材中不同方向纵向排列，顺序为上左下右
bool CHARACTOR::BuildAnimeIndex(int directionNum, int num1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
	int offset = 0;
	for (int i = 0; i < directionNum; i++)//方向的数量（4/8）
	{
		animeIndex[i][0] = i;
	}
	if (directionNum < 8)
	{
		animeIndex[directionNum][0] = 0;//尾端置0
	}
	if (directionNum > 1 && n2 == 0)
	{
		for (int i = 0; i < directionNum; i++)
		{
			animeIndex[i][1] = offset;//总偏移
			animeIndex[i][2] = num1;//有几个图片
			offset += num1;
		}
	}
	else
	{
		offset = num1 + n2 + n3 + n4 + n5 + n6 + n7;
		switch (directionNum)
		{
		case 8:
			animeIndex[7][1] = offset;
			animeIndex[7][2] = n8;
			offset -= n7;
		case 7:
			animeIndex[6][1] = offset;
			animeIndex[6][2] = n7;
			offset -= n6;
		case 6:
			animeIndex[5][1] = offset;
			animeIndex[5][2] = n6;
			offset -= n5;
		case 5:
			animeIndex[4][1] = offset;
			animeIndex[4][2] = n5;
			offset -= n4;
		case 4:
			animeIndex[3][1] = offset;
			animeIndex[3][2] = n4;
			offset -= n3;
		case 3:
			animeIndex[2][1] = offset;
			animeIndex[2][2] = n3;
			offset -= n2;
		case 2:
			animeIndex[1][1] = offset;
			animeIndex[1][2] = n2;
			offset = 0;
		case 1:
			animeIndex[0][1] = offset;
			animeIndex[0][2] = num1;
		default:
			break;
		}
	}
	return true;
}

bool CHARACTOR::Move(int InputDirection, float deltaTime)
{
	float prevX = position[0], prevY = position[1];
	float moveRange = moveSpeed*deltaTime;
	if (target[0] != -1)
	{
		float targX = abs(position[0] - target[0]), targY = abs(position[1] - target[1]);
		switch (InputDirection)
		{
		case DIRECTION_UP:
			position[1] -= targY<moveRange?targY:moveRange;
			break;
		case DIRECTION_LEFT:
			position[0] -= targX<moveRange ? targX : moveRange;
			break;
		case DIRECTION_DOWN:
			position[1] += targY<moveRange ? targY : moveRange;
			break;
		case DIRECTION_RIGHT:
			position[0] += targX<moveRange ? targX : moveRange;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (InputDirection)
		{
		case DIRECTION_UP:
			position[1] -= moveRange;
			break;
		case DIRECTION_LEFT:
			position[0] -= moveRange;
			break;
		case DIRECTION_DOWN:
			position[1] += moveRange;
			break;
		case DIRECTION_RIGHT:
			position[0] += moveRange;
			break;
		default:
			break;
		}
	}
	if (!IsPositionMoveable(position[0], position[1], this))
	{
		position[0] = prevX, position[1] = prevY;
		FindNearestPos();
		return false;
	}
	return true;
}

void CHARACTOR::CharaAnimate(float deltaTime)
{
	if (isMannualMoving)//手动移动
	{
		isMannualMoving = false;
		if(-1!= target[0])
			target[0] = target[1] = -1;
		Move(faceDirection, deltaTime);
		animateTick += deltaTime*1000;
		//如果经过时间大于帧率则更新动画帧
		if (animateTick > delay)
		{
			int dir = faceDirection;
			int index = animeIndex[dir][0];
			int startFrame = animeIndex[index][1], endFrame = animeIndex[index][1] + animeIndex[index][2] - 1;
			animateTick -= delay;
			frame += direction;
			if (frame > endFrame)
				frame = startFrame;
			else if (frame < startFrame)
				frame = endFrame;
		}
	}
	else if(-1!=target[0])//自动移动
	{
		//要移动的目的地距离
		float moveX = (target[0] - position[0]), moveY = (target[1] - position[1]);
		if (moveX == 0.00f && moveY == 0.00f)//不移动说明到达
		{
			target[0] = target[1] = -1;
		}
		else//移动方向判定
		{
			if (abs(moveX) >= abs(moveY))//左或右，在线上则视为左右
			{
				faceDirection = moveX > 0 ? 3 : 1;//按X正负区分左右
			}
			else
			{
				faceDirection = moveY > 0 ? 2 : 0;//按Y区分上下
			}
			//读取当前的显示帧范围
			int dir = faceDirection;
			int index = animeIndex[dir][0];
			int min = animeIndex[index][1];
			int max = animeIndex[index][1] + animeIndex[index][2] - 1;
			if (frame > max || frame < min)
			{
				frame = min;
				animateTick = 0.0f;
			}
			Move(faceDirection, deltaTime);
			animateTick += deltaTime * 1000;
			//如果经过时间大于帧率则更新动画帧
			if (animateTick > delay)
			{
				int startFrame = animeIndex[index][1], endFrame = animeIndex[index][1] + animeIndex[index][2] - 1;
				animateTick -= delay;
				frame += direction;
				if (frame > endFrame)
					frame = startFrame;
				if (frame < startFrame)
					frame = endFrame;
			}
		}
	}
	else//不 动 明 王
	{
		//设置面朝当前方向并显示停下的帧(如果更改了方向和顺序的关系，则animeIndex[0][0]为向上的帧所在行编号，[1][0]为左编号,以此类推，这里为范例）
		int dir = faceDirection;
		int index = animeIndex[dir][0];
		frame = animeIndex[index][1];
	}
	if (attackCD > 0.0f)
	{
		attackCD -= deltaTime;
		if (attackCD < 0.0f)
			attackCD = 0.0f;
	}
	if (MP < MaxMP)
	{
		MPrecoverCD -= deltaTime;
		if (MPrecoverCD <= 0.0f)
		{
			if (MP < MaxMP)
			{
				MP += 1;
				MPrecoverCD = 1.0f;
			}
			else
			{
				MPrecoverCD = 0.0f;
			}
		}	
	}
}

void CHARACTOR::DrawGUI()
{
	//左侧坐标
	int left = (int)(x + position[0] - ScrollX);
	//右侧坐标
	int right = (int)(left + width*scaling);
	//高度
	int down = (int)(y + position[1] - ScrollY);
	int up = down - (int)(height*scaling*0.15f);
	RECT playerMP{ left,up,right,down };
	//绘制底部的黑色
	RECT playerHP{ left,(LONG)(up - height*scaling*0.15 - 1),right,up - 1 };
	d3ddev->ColorFill(backbuffer, &playerHP, D3DCOLOR_XRGB(0, 0, 0));
	d3ddev->ColorFill(backbuffer, &playerMP, D3DCOLOR_XRGB(0, 0, 0));
	//绘制血条/蓝
	float notDivideZero0 = 1.0f, notDivideZero1 = 1.0f;
	if ((float)MaxHP > 0.0f)
		notDivideZero0 = (float)MaxHP;
	if ((float)MaxMP > 0.0f)
		notDivideZero1 = (float)MaxMP;
	playerHP.right = playerHP.left + (LONG)(width*scaling*(HP / notDivideZero0 <= 1.0f ? HP / notDivideZero0 : 1.0f));
	playerMP.right = playerMP.left + (LONG)(width*scaling*(MP / notDivideZero1 <= 1.0f ? MP / notDivideZero1 : 1.0f));
	d3ddev->ColorFill(backbuffer, &playerHP, D3DCOLOR_XRGB(200, 0, 0));
	d3ddev->ColorFill(backbuffer, &playerMP, D3DCOLOR_XRGB(0, 47, 167));
}

CHARACTOR * CHARACTOR::GetNearbyChara(int range)
{
	CHARACTOR* p=NULL;
	RECT rect = { 0,0,0,0 };
	switch (faceDirection)
	{
	case DIRECTION_UP:
		rect = { (long)position[0] - range, (long)position[1] - range, (long)position[0] + range , (long)position[1] };
		break;
	case DIRECTION_LEFT:
		rect = { (long)position[0] - range, (long)position[1] - range, (long)position[0] , (long)position[1] + range };
		break;
	case DIRECTION_DOWN:
		rect = { (long)position[0] - range, (long)position[1] , (long)position[0] + range , (long)position[1] + range };
		break;
	case DIRECTION_RIGHT:
		rect = { (long)position[0] , (long)position[1] - range, (long)position[0] + range , (long)position[1] + range };
		break;
	default:
		rect = { (long)position[0] - range, (long)position[1] - range, (long) position[0] + range , (long) position[1] + range };
		break;
	}
	//不是vector内元素的引用，而是新的栈变量，返回后变野指针
	//for each (auto var in constantNPCs) {}
	//返回的是迭代器类型，不能为空
	//for (auto var = constantNPCs.begin(); var < constantNPCs.end(); var++){}
	//不是vector内元素的引用，新的栈变量也是返回野指针
	//for(auto var:constantNPCs){	}
	//C++11新函数，对一些collection(vector,list,set,map)中元素执行function()
	//for_each(iterator first, iterator last, function());
	//无奈之下最原始的操作
	for (int i = 0; i<(int)constantNPCs.size(); i++)
	{
		//利用引用不需要解引用的特性
		CHARACTOR& var = constantNPCs[i];
		//跳过自己（自己和自己的距离永远是0）
		if (&var == this)
			continue;
		//
		if (var.position[0] >(float)rect.left&&var.position[0] < (float)rect.right&&var.position[1] > (float)rect.top&&var.position[1]< (float)rect.bottom)
		{
			if (p == NULL)
			{
				p = &var;
			}
			else if (pow(p->position[0] - position[0], 2) + pow(p->position[1] - position[1], 2)>pow(var.position[0] - position[0], 2) + pow(var.position[1] - position[1], 2))
			{
				p = &var;
			}//
		}
	}
	return p;
}

bool CHARACTOR::Damage(int i,int type)
{

	switch (type)
	{
	case 0:
		HP -= i;
		damageNums.addDamageNum(i, position[0] - ScrollX, position[1] - ScrollY);
		if (HP <= 0)
		{
			//Dead
			HP = 0;
			hermes.notify(*this, EVENT_KILL_MONSTER);
			return false;
		}
		else
		{
			hermes.notify(*this, EVENT_INJURED);
			return true;
		}
		break;
	case 1:
		MP -= i;
		if (MP <= 0)
		{
			//Dead
			MP = 0;
			return false;
		}
		else
		{
			return true;
		}
		break;
	default:
		return false;
		break;
	}
	
}

bool CHARACTOR::Heal(int i, int type)
{
	switch (type)
	{
	case 0:
		HP += i;
		if (HP >= MaxHP)
		{
			HP = MaxHP;
			return false;
		}
		else
		{
			return true;
		}
		break;
	case 1:
		MP += i;
		if (MP >= MaxMP)
		{
			MP = MaxMP;
			return false;
		}
		else
		{
			return true;
		}
		break;
	default:
		return false;
		break;
	}
}

void ShowDamage(int damage,int x,int y)
{

}