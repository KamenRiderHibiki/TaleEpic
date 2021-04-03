

#include "DirectX.h"
#include "InGameObject.h"

int (WINAPIV * __vsnprintf)(char *, size_t, const char*, va_list) = _vsnprintf;

#define MAP_SOURCE_NUM 10		//��ͼ��Դͼ������
#define PARTICLE_NUMBER  200	//��������
#define TILE_PIXEL_NUM 32		//ÿ����Ƭ�Ŀ�ȣ����أ�

#define DIRECTION_UP 0			//��
#define DIRECTION_LEFT 1		//��
#define DIRECTION_DOWN 2		//��
#define DIRECTION_RIGHT 3		//��

const string APPTITLE = "Test Zone";//����
int SCREENW = 1024;//��Ļ��ʼ���
int SCREENH = 768;//��Ļ��ʼ�߶�
int ScrollX = 0, ScrollY = 0;//�����ͼ�����Ͻ�λ��
int OffsetX = 0, OffsetY = 0;//��ͼ���ƻ�������Ͻ�λ��
int MaxMapX = 0, MaxMapY = 0;//��ͼ�߽�
int wildEnemyNum = 0;
double wildSpawnCD = EMY_SPN_CD;
GameState gamestate = GameState::Init;
bool gameStart = false;
bool snowShow = false;

//��ͼ����Ϣ
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

//ѩ�����ӽṹ��Ķ���
struct PARTICLE
{
	float x, y;      //����λ��
	float width, height,maxWidth,maxHeight;//��С
	float RotationY;         //ѩ��������Y����ת�Ƕ�
	float RotationX;       //ѩ��������X����ת�Ƕ�
	float FallSpeed;       //ѩ���½��ٶ�
	float WindSpeed;       //ѩ����ת�ٶ�
	float scale;           //����
	int   TextureIndex;     //�����������
	PARTICLE()
	{
		x = y = width = height = RotationX = RotationY = WindSpeed = 0.0f;
		TextureIndex = 0;
		scale = 1.0f;
	}
};

//����ϵͳ��Ķ���
class ParticleClass
{
private:
	PARTICLE m_Snows[PARTICLE_NUMBER];    //��������
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;      //�����������ݵĶ��㻺��
	LPDIRECT3DTEXTURE9 m_pTexture[6];  //ѩ������

public:
	ParticleClass(LPDIRECT3DDEVICE9 d3ddev);   //���캯��
	~ParticleClass();                                  //��������
	HRESULT InitParticle(HWND window);        //����ϵͳ��ʼ������
	HRESULT UpdateParticle(float fElapsedTime);   //����ϵͳ���º���
	HRESULT RenderParticle();   //����ϵͳ��Ⱦ����
};

ParticleClass::ParticleClass(LPDIRECT3DDEVICE9 d3ddev)
{

}

ParticleClass::~ParticleClass()
{
}

//����ϵͳ��ʼ������
HRESULT ParticleClass::InitParticle(HWND window)
{
	D3DXVECTOR2 size[6];
	//����6��ѩ������
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
	//��ʼ��ѩ����������
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

//����ϵͳ���º���
HRESULT ParticleClass::UpdateParticle(float fDeltaTime)
{
	//һ��forѭ��������ÿ��ѩ�����ӵĵ�ǰλ�úͽǶ�
	for (int i = 0; i<PARTICLE_NUMBER; i++)
	{
		m_Snows[i].y += m_Snows[i].FallSpeed*fDeltaTime;
		m_Snows[i].x += m_Snows[i].WindSpeed*fDeltaTime;
		//����λ��
		//���ѩ�������䵽����, ���½���߶�����Ϊ���
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
		//���´�С
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

//�����ӡ�����
void ParticleDraw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, float rotation, float scaling1, float scaling2, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255))
{
	D3DXVECTOR2 scale(scaling1, scaling2);//xy��������
	D3DXVECTOR2 trans(x+(width-width*scaling1)/2, y+(height-height*scaling2)/2);//xyƫ��
	D3DXVECTOR2 center((float)(width*scaling1) / 2, (float)(height*scaling2) / 2);//��ת����
	D3DXMATRIX mat;//�任����
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);//�趨�任����
	spriteobj->SetTransform(&mat);
	int fx = 0;
	int fy = 0;
	RECT srcRect = { fx, fy, fx + width, fy + height };
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);

	D3DXMatrixIdentity(&mat);//������λ����
	spriteobj->SetTransform(&mat);//�ָ���������ϵ
}

HRESULT ParticleClass::RenderParticle()
{
	//��Ⱦѩ��
	for (int i = 0; i<PARTICLE_NUMBER; i++)
	{
		ParticleDraw(m_pTexture[m_Snows[i].TextureIndex], (int)(m_Snows[i].x-ScrollX- m_Snows[i].maxWidth/2), (int)(m_Snows[i].y-ScrollY - m_Snows[i].maxHeight / 2), (int)m_Snows[i].maxWidth, (int)m_Snows[i].maxHeight,0.0f, (m_Snows[i].width/ m_Snows[i].maxWidth)*m_Snows[i].scale, (m_Snows[i].height/ m_Snows[i].maxHeight)*m_Snows[i].scale);
	}

	return S_OK;
}

//��ͼ������Ϣ
struct MAPINDEX
{
	int number = 0;//�ؿ����
	int tilenum = 0;//�ؿ�����
	MAPINDEX() = delete;
	MAPINDEX(int m, int s)
	{
		number = m;
		tilenum = s;
	}
};
//��ͼ������Ϣ
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
//��ͼ����ļ���
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
//���弰��Ȩ
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


//�ж��ܷ��ƶ�
bool IsPositionMoveable(float x, float y, CHARACTOR* chara = nullptr);

string getSpeciesFromTerrian(int x, int y);
//���ƽ�ɫ
bool DrawCharacters(vector<CHARACTOR> c)
{
	for each (CHARACTOR var in c)
	{
		SpriteTransformDraw(*var.sprite, (int)(var.x + var.position[0] - ScrollX), (int)(var.y + var.position[1] - ScrollY), var.width, var.height, var.frame, var.columns, 0.0f, var.scaling);
		var.DrawGUI();
	}
	return false;
}

//gbk��utf8����ת�� ��Դ��https://blog.csdn.net/u012234115/article/details/83186386
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
	//��ȡ�ļ�
	ifstream ReadFile;
	ReadFile.open(filepath);
	if (ReadFile.fail())
	{
		cout << "open json file failed." << endl;
		return result;
	}
	//װ������
	json j;
	ReadFile >> j;
	//��ȡ�汾
	float version = j["version"];
	cout << version << endl;
	//��ȡ������������
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
//���
extern LPPOINT lpPoint;
//��ͼ��Դλ��
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
MAPTILEINF mapSourceInf[MAP_SOURCE_NUM];//��ͼ��Ϣ
map<string, vector<SPECIESPRO>> mapSpeciesInf;//���ι�����Ϣ
vector<LPDIRECT3DTEXTURE9> mapSource;//��ͼͼ��

SPRITE akashi;
//SPRITE showDamage;
//������Դ
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
//ͼ��
LPDIRECT3DTEXTURE9 imgknight0_icon = NULL;
LPDIRECT3DTEXTURE9 imgglassman_icon = NULL;
LPDIRECT3DTEXTURE9 imgloli_icon = NULL;
LPDIRECT3DTEXTURE9 imgprincess_icon = NULL;
LPDIRECT3DTEXTURE9 imgstartGame_icon = NULL;
//������Դ
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
LPDIRECT3DSURFACE9 gameworld = NULL;//��Ϸ��ͼ

LPD3DXFONT fontArial36 = NULL;
LPD3DXFONT fontTimesNewRoman40 = NULL;
ParticleClass*	g_pSnowParticles = NULL;    //ѩ������ϵͳ��ָ��ʵ��
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

//��ȡ��ͼ����
MAP loadMapFromJson(string filepath)
{
	MAP map;
	//��ȡ�ļ�
	ifstream ReadFile;
	ReadFile.open(filepath);
	if (ReadFile.fail())
	{
		cout << "open json file failed." << endl;
		return map;
	}
	//װ������
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
		//��ȡÿ���ͼ������
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
				//װ��mapindex����
				int tilenum = temp["data"][j];
				mapblock.mapindex.push_back(MAPINDEX(j, tilenum-1));
			}
			//װ��mapblock
			map.maplist.push_back(mapblock);
		}
	}
	return map;
}
//��ȡ��ͼ�Ŀ��ƶ���Ϣ
bool loadObstacleFromJson(string texpath, vector<short>& obdata,string& terrianType)
{
	string str = texpath.replace(texpath.find(".") + 1, 4, "json");
	//��ȡ�ļ�
	ifstream ReadFile;
	ReadFile.open(str);
	if (ReadFile.fail())
	{
		//���������Ϣ
		cout << "open json file failed." << endl;
		return false;
	}
	//װ������
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
//��ȡ������Ϣ
vector<MISSION> loadMissionFromJson(string path)
{
	//��ȡ�ļ�
	ifstream ReadFile;
	ReadFile.open(path);
	if (ReadFile.fail())
	{
		//���������Ϣ
		cout << "open json file failed." << endl;
		//return false;
	}
	//װ������
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
			�������Ԫ��Ȼ��Ҫ����������
			�������Խ��<С�ڵ����ȼ�λ�ڵ�8�飬��:?��Ԫ����������ȼ�λ�ڵ�15�飬��ˡ�k < tkSize < 2����������С�ڽ������㡣
			С�ڷ��Ŵ����ҽ�ϣ�false��Ϊ0��true��Ϊ1((false == 0)==true,true<1==false,true<2==true)�����ߵ����㡣������
			0 < tkSize < 1 == false;
			3 < tkSize < 1 == true;
			������true����false,tkSize��2��������������¶�����true( >0 ),���Ի�����ѭ��
			*/
			json tempTalk = tempMission["talk"][k];
			//δ����
			auto naSize = tempTalk["notaccept"].size();
			for (int nat = 0; nat < naSize; nat++)//NotAcceptTalks
			{
				json tk = tempTalk["notaccept"][nat];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				//m.talks[k]["notaccept"][nat] = t;
				m.talks[k]["notaccept"].push_back(t);//����.talk[������/������][״̬][�������]
			}
			//������
			auto uwSize = tempTalk["underway"].size();
			for (int uwt = 0; uwt < uwSize; uwt++)
			{
				json tk = tempTalk["underway"][uwt];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				m.talks[k]["underway"].push_back(t);
			}
			//�����
			auto clSize = tempTalk["complete"].size();
			for (int clt = 0; clt < clSize; clt++)
			{
				json tk = tempTalk["complete"][clt];
				string name = tk["name"];
				string sent = tk["sentence"];
				TALK t = TALK(Utf8ToGbk(name.c_str()), Utf8ToGbk(sent.c_str()));
				m.talks[k]["complete"].push_back(t);
			}
			//�ѽ���
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
//��ʼ����Ƭ��Դ
bool InitMapSource(vector<LPDIRECT3DTEXTURE9> &mapSource,HWND window)
{
	int i = 0;
	//��mapSourcePaths·��װ�ص�ͼ����Դ
	for each (string var in mapSourcePaths)
	{
		LPDIRECT3DTEXTURE9 ms = LoadTexture(var);
		//��ȡÿ����Դ��ͼƬ����
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
			//˳����ȡ�ϰ���͵���������Ϣ
			loadObstacleFromJson(var, mapSourceInf[i].obstacles, mapSourceInf[i].terrian);
		}
		i++;
	}
	return true;
}

//�齨��ͼ
LPDIRECT3DSURFACE9 BuildWorldMap(MAP& map,CHARACTOR player)
{
	int left = ScrollX,up=ScrollY,right=left+SCREENW,down=up+SCREENH;
	RECT screenRect={left,up,right,down};
	gameworldRect = screenRect;
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 100, 200), 1.0f, 0);
	spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
	//����ÿһ����ͼ��
	for (int i = 0; i < (int)map.maplist.size(); i++)
	{
		MAPBLOCK& m = map.maplist[i];
		//ͼ����Դ���
		int mapSourceIndex = m.mapSourceIndex;
		//��Ӧλ�õ�RECT
		RECT mapblockRect = { m.x,m.y,m.x + m.width*TILE_PIXEL_NUM,m.y + m.height*TILE_PIXEL_NUM };
		//������mapblock����Ҫ���ƵĲ���
		if (RECTContain(screenRect, mapblockRect))
		{
			for (int j = 0; j < (int)m.mapindex.size(); j++)
			{
				MAPINDEX& index = m.mapindex[j];
				if (-1 == index.tilenum)
				{
					continue;
				}
				//����Ƿ�Ҫ���������Ƭ
				long left1 = m.x + (index.number%m.width)*TILE_PIXEL_NUM, top1 = m.y + (index.number / m.width)*TILE_PIXEL_NUM;
				RECT indexRect = { left1,top1,left1 + TILE_PIXEL_NUM,top1 + TILE_PIXEL_NUM };
				if (RECTContain(screenRect, indexRect))
				{
					//���Ƶ�Ŀ�������
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
//��ȡ���������Ļ����
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
	if (gt.DeltaTime() == -1.0f)//��GameInit()��ĵ���
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
//���Ƶ�ͼ
bool DrawCurrentMap(CHARACTOR player, GameTimer &gt)
{
	//��ȡ��ǰ��ͼ��λ��
	RECT screenRect = GetScreenRect(player, gt);
	BuildWorldMap(earth_map, player);
	return true;
}
//��ȡһ������Ĺ������λ��
VEC getEnemyPos(CHARACTOR& player, GameTimer &mTimer)
{
	//������������Ļ��Χ�ڣ�
	RECT inner = GetScreenRect(player, mTimer);
	//������
	RECT outer = RECT{ inner.left - EMY_SPN_DIS > 0 ? inner.left - EMY_SPN_DIS : 0,inner.top - EMY_SPN_DIS > 0 ? inner.top - EMY_SPN_DIS : 0,inner.right + EMY_SPN_DIS > 0 ? inner.right + EMY_SPN_DIS : 0,inner.bottom + EMY_SPN_DIS > 0 ? inner.bottom + EMY_SPN_DIS : 0 };
	//�������0-1�����
	float x0 = rand() / double(RAND_MAX);
	float y0 = rand() / double(RAND_MAX);
	//����Ļ��С�������λ��
	int x = x0 * (outer.right - outer.left);
	int y = y0 * (outer.bottom - outer.top);
	//��������ڲ�������
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
		00��2���ޣ�	|  01��1���ޣ�
		��������������������������������
		10��3���ޣ�	|  11��4���ޣ�


		*/
		//��Ȼ�п���һ����0��������������
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

//��������Ķ�ȡ����
void CheckInput(HWND window,string& text, GameTimer &mTimer) {
	auto nowTime = mTimer.TotalTime();
	
	if (MouseButton(0))
	{
		//������ҵ�����Ŀ��
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
			//������ϴδ��ڹر�ʱ��С��0.5��ͽ�ֹ�ƶ�����ֹ�رմ��ڵĵ���¼�©���´�ѭ������ƶ�Ŀ�������
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
				//��ȡ��ײ�Dialog
				DIALOG& d = newDialogs[(int)newDialogs.size() - 1];
				if (d.isClicked(lpPoint->x, lpPoint->y) && (nowTime - DIALOG::LastDialogTime > 0.5f))
				{
					newDialogs.erase(newDialogs.end() - 1, newDialogs.end());
					//�����һ�ιر����д��ڵ�ʱ�������
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
				//�����ж�
				float distanceX = (player->position[0] - p->position[0]), distanceY = (player->position[1] - p->position[1]);
				if (abs(distanceX) >= abs(distanceY))//����ң�����������Ϊ����
				{
					p->faceDirection = distanceX > 0 ? 3 : 1;//��X������������
				}
				else
				{
					p->faceDirection = distanceY > 0 ? 2 : 0;//��Y��������
				}
				//��ȡ��ǰ����ʾ֡��Χ
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
				//�����ж�
				float distanceX = (player->position[0] - p->position[0]), distanceY = (player->position[1] - p->position[1]);
				if (abs(distanceX) >= abs(distanceY))//����ң�����������Ϊ����
				{
					p->faceDirection = distanceX > 0 ? 3 : 1;//��X������������
				}
				else
				{
					p->faceDirection = distanceY > 0 ? 2 : 0;//��Y��������
				}
				//��ȡ��ǰ����ʾ֡��Χ
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
						//δ����״̬
						if (m.eventState == MissionState::NotAccept)
						{
							//��ȡDialog
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
							//�ı�����״̬
							m.eventState = MissionState::Underway;
						}
						//������
						else if (m.eventState == MissionState::Underway)
						{
							//��ȡDialog
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
						//�����
						else if (m.eventState == MissionState::Complete)
						{
							//��ȡDialog
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
							//��ȡDialog
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
							//��ȡDialog
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
						//�����
						else if (m.eventState == MissionState::Complete)
						{
							//��ȡDialog
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
					string s = "�ҵ�������" + p->tag + "����ʲô����";
					if (p->tag == "knight")
					{
						s = "��л���������ڲ����±��������ˡ�";
					}
					else if (p->tag == "manwithglass")
					{
						s = "��������𣿱�Ǹ��Ҫ������Ʊ��";
					}
					
					//string s = "С�㣬�����µĴ���Ҫ��Ʊ�������а�����û���ˡ�";
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

	p = CHARACTOR(SpeciesMap.at("Լ������"), spriteInfos.at("merchant"), 1, D3DVECTOR{ 9250,2600,0 }, "manwithglass", true);
	p.icon = imgglassman_icon;
	constantNPCs.push_back(p);

	npc0 = CHARACTOR(SpeciesMap.at("��������"), spriteInfos.at("knight"), 1, D3DVECTOR{ 3750,1610,0 }, "knight", true);
	npc0.icon = imgknight0_icon;
	constantNPCs.push_back(npc0);

	npc1 = CHARACTOR(SpeciesMap.at("Լ������"), spriteInfos.at("loli"), 1, D3DVECTOR{ 2350,2650,0 }, "loli", true);
	npc1.icon = imgloli_icon;
	constantNPCs.push_back(npc1);

	npc2 = CHARACTOR(SpeciesMap.at("��������"), spriteInfos.at("princess"), 1, D3DVECTOR{ 12450,1150,0 }, "princess", true);
	npc2.icon = imgprincess_icon;
	constantNPCs.push_back(npc2);
}
//��Ϸ��ʼ��
bool GameInit(HWND window)
{
	//����Direct3D���(�����������Ļ�ֱ���Ϊ1080P)
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
	//����λ�����ö�λָ��
	AudioLocator::setService(&audio);
	CharaLocator::setService(&constantNPCs);
	MissionSystemLocator::setService(&eventsystem);
	//װ��ͼ��
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
	//������Ƶ
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
	
	sound_icefield_bg = LoadSound("Sources/Sounds/���ѩ������.wav");
	if (!sound_icefield_bg)
	{
		MessageBox(window, "Error loading ���ѩ������.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("icefieldBGM", sound_icefield_bg);
	}
	sound_village_bg = LoadSound("Sources/Sounds/�ä�ʼ�ޤ�.wav");
	if (!sound_village_bg)
	{
		MessageBox(window, "Error loading �ä�ʼ�ޤ�.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("villageBGM", sound_village_bg);
	}
	sound_seaside_bg = LoadSound("Sources/Sounds/ˬ�䤫���L.wav");
	if (!sound_seaside_bg)
	{
		MessageBox(window, "Error loading ˬ�䤫���L.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("seasideBGM", sound_seaside_bg);
	}
	sound_forest_bg = LoadSound("Sources/Sounds/ɭ���l�����L.wav");
	if (!sound_forest_bg)
	{
		MessageBox(window, "Error loading ɭ���l�����L.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("forestBGM", sound_forest_bg);
	}
	sound_mainmenu_bg = sound_icefield_bg;
	if (!sound_mainmenu_bg)
	{
		MessageBox(window, "Error loading ���L.wav", APPTITLE.c_str(), 0);
		//return false;
	}
	else
	{
		audio.addSound("mainmenuBGM", sound_mainmenu_bg);
	}
	//����Ϸϵͳ�Ĵ����˼������۲���
	hermes.addObserver(&eventsystem);
	hermes.addObserver(&audioManager);

	//��������
	fontArial36 = MakeFont("Arial", 36);
	fontTimesNewRoman40 = MakeFont("TimesNewRoman", 40);
	//������Ϸ����
	SpeciesMap = loadSpeciesMapFronJson("Sources/GameData.json");
	eventsystem.setMissions(loadMissionFromJson("Sources/missions.json"));
	//���ؾ���
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

	//���ؽ�ɫ
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
	
	//��ʱtimer���󣬸�GetScreenRect������
	GameTimer gt;
	gt.Reset();
	gt.Start();
	//װ�ص�ͼ��Դ
	InitMapSource(mapSource, window);
	//�༭��ͼ���͵Ĺ���
	SPECIESPRO s=SPECIESPRO("boar",1.0f);
	vector<SPECIESPRO> vs;
	vs.push_back(s);
	mapSpeciesInf["Snowfield"] = vs;
	//װ�ص�ͼ
	earth_map += loadMapFromJson("village.json");
	earth_map += loadMapFromJson("test1.json");
	earth_map += loadMapFromJson("test3.json");
	//earth_map += loadMapFromJson("test4.json");
	earth_map += loadMapFromJson("seaside_right.json");
	earth_map += loadMapFromJson("wood.json");
	earth_map += loadMapFromJson("temple.json");

	//��ȡmaxMapX��maxMapY
	for each (MAPBLOCK var in earth_map.maplist)
	{
		int x = var.x + var.width * 32;
		MaxMapX = x > MaxMapX ? x : MaxMapX;
		int y = var.y + var.height * 32;
		MaxMapY = y > MaxMapY ? y : MaxMapY;
	}
	//�������vector������capcity���Զ����ݻ��player���Ұָ��
	player = GetPlayer(constantNPCs, "loli");
	//��ȡ��Ļ
	GetScreenRect(*player, gt);

	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	//��������ʼ��ѩ������ϵͳ
	g_pSnowParticles = new ParticleClass(d3ddev);
	g_pSnowParticles->InitParticle(window);
	gamestate = GameState::menu;
	
	AudioLocator::getAudio()->loopSound("mainmenuBGM");

	return true;
}
//��Ϸ����
void GameRun(HWND window,GameTimer &mTimer)
{
	if (!d3ddev)
		return;
	DirectInputUpdate();
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET || D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 200), 1.0f, 0);
	
	//debug��Ϣ
	string text = "";
	
	//��������
	CheckInput(window,text,mTimer);
	//�߼�����
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
	//���´���
	if (gamestate == GameState::menu)
	{
		nonGameDialogs.at("menu").update(CHARACTOR());
	}
	else if (gamestate == GameState::play_ing || GameState::play_dialog)
	{
		//spawnҰ��
		if ((wildEnemyNum<EMY_SPN_NUM) && ((wildSpawnCD -= mTimer.DeltaTime()) <= 0.0))
		{
			wildSpawnCD = EMY_SPN_CD;
			string playerTag = player->tag;
			//SPRITEINF b1 = SPRITEINF(imgboar, 4, 1.0f, 0, 15, 99);
			VEC pos = getEnemyPos(*player, mTimer);
			string species = getSpeciesFromTerrian((int)pos.x, (int)pos.y);
			if (species == "boar")
			{
				CHARACTOR beast0 = CHARACTOR(SpeciesMap.at("Ұ��"), spriteInfos.at("boar"), 1, D3DVECTOR{ pos.x,pos.y,0 }, "boar");
				constantNPCs.push_back(beast0);
				player = GetPlayer(constantNPCs, playerTag);
				wildEnemyNum++;
			}
		}
		//��������ķ����
		for (auto iter = constantNPCs.begin(); iter != constantNPCs.end();)
		{
			//���õ����������˽����������*���ص� ��Դ��https://blog.csdn.net/hl_zzl/article/details/84575713
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
		
		//������ı仯
		//SpriteAnimate(akashi.frame, akashi.startframe, akashi.endframe, akashi.direction, akashi.starttime, akashi.delay);
		akashi.x = ScrollX, akashi.y = ScrollY;
		//���½�ɫ״̬
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
	//text = "ɱ�����Ϊ" + to_string(eventsystem.getKillNum());
	//text = to_string(player->MPrecoverCD)+","+ to_string(player->MP);
	
	//���ƴ���
	if (d3ddev->BeginScene())
	{
		switch (gamestate)
		{
		case GameState::menu:
			spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
			//��ʼ����
			nonGameDialogs.at("menu").show();
			if (text != "")
				FontPrint(fontTimesNewRoman40, 0, 200, text, D3DCOLOR_XRGB(0, 0, 100));
			//��������
			spriteobj->End();
			break;
		case GameState::play_ing:
		case GameState::play_dialog:
		case GameState::play_pause:
			DrawCurrentMap(*player, mTimer);
			spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
			//��ʼ����

			//����ѩ������ϵͳ
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
			//��������
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
//��Ϸ����
void GameEnd()
{
	SafeRelease(imgakashi);
	//�ͷ�Direct���
	DirectSoundShutdown();
	DirectInputShutdown();
	Direct3DShutdown();
}

bool saveGame(string path)
{
	
	json j;
	//��ɫ
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
	//����
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
	//��ȡDialog
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
	//��ȡDialog
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
	//����ľ�������
	int absX = x, absY = y ;
	//��鱻������ĸ���
	for each (MAPBLOCK var in earth_map.maplist)
	{
		RECT rect= { var.x,var.y,var.x + var.width*TILE_PIXEL_NUM,var.y + var.height*TILE_PIXEL_NUM };
		if (absX > rect.left&&absX<rect.right&&absY>rect.top&&absY < rect.bottom)
		{
			//����Ļ�з���ļ��м���
			int col = (absX-var.x) / TILE_PIXEL_NUM, row = (absY-var.y) / TILE_PIXEL_NUM;
			int tileNum = var.mapindex[row*var.width+col].tilenum;
			//����õ�û�ж��壨�������mapblock��
			if (-1 == tileNum)
			{
				continue;
			}
			//����õ��ǲ���ͨ�е�
			else if(1 == mapSourceInf[var.mapSourceIndex].obstacles[tileNum])
			{
				result = false;
			}
			//����õ��ǳ�ͨ�п飨������ʹ�õײ��Ǻ��󡢺���Ҳ��ͨ�У�
			else if (2 == mapSourceInf[var.mapSourceIndex].obstacles[tileNum])
			{
				result = true;
				break;
			}
			//�����ͨ�У�=0�����������ã�Ҫ���Ƿ��������������������
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
			//����Ļ�з���ļ��м���
			int col = (x - var.x) / TILE_PIXEL_NUM, row = (y - var.y) / TILE_PIXEL_NUM;
			int tileNum = var.mapindex[row*var.width + col].tilenum;
			//���û�ж���
			if (-1 == tileNum)
			{
				continue;
			}
			//��ȡ��Դ
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
		//�ܹ��ļ���
		float i = 0.0f;
		for each (auto var in a)
		{
			i += var.probability;
		}
		float scaleCon = 1.0f / i;
		//������      *��������ڵ�λ��
		//0������1��/0.25������2��/0.75������3��/1
		vector<int> species(a.size(), 0);
		int probab = 0;
		for (auto i = 0; i < (int)species.size(); i++)
		{
			probab += scaleCon*a[i].probability * 1000;
			species[i] = probab;
		}
		*species.rbegin() = 1000;//���ļ���Ϊ100%

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
//�齨����֡������������ٶ����ز��в�ͬ�����������У�˳��Ϊ��������
bool CHARACTOR::BuildAnimeIndex(int directionNum, int num1, int n2, int n3, int n4, int n5, int n6, int n7, int n8)
{
	int offset = 0;
	for (int i = 0; i < directionNum; i++)//�����������4/8��
	{
		animeIndex[i][0] = i;
	}
	if (directionNum < 8)
	{
		animeIndex[directionNum][0] = 0;//β����0
	}
	if (directionNum > 1 && n2 == 0)
	{
		for (int i = 0; i < directionNum; i++)
		{
			animeIndex[i][1] = offset;//��ƫ��
			animeIndex[i][2] = num1;//�м���ͼƬ
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
	if (isMannualMoving)//�ֶ��ƶ�
	{
		isMannualMoving = false;
		if(-1!= target[0])
			target[0] = target[1] = -1;
		Move(faceDirection, deltaTime);
		animateTick += deltaTime*1000;
		//�������ʱ�����֡������¶���֡
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
	else if(-1!=target[0])//�Զ��ƶ�
	{
		//Ҫ�ƶ���Ŀ�ĵؾ���
		float moveX = (target[0] - position[0]), moveY = (target[1] - position[1]);
		if (moveX == 0.00f && moveY == 0.00f)//���ƶ�˵������
		{
			target[0] = target[1] = -1;
		}
		else//�ƶ������ж�
		{
			if (abs(moveX) >= abs(moveY))//����ң�����������Ϊ����
			{
				faceDirection = moveX > 0 ? 3 : 1;//��X������������
			}
			else
			{
				faceDirection = moveY > 0 ? 2 : 0;//��Y��������
			}
			//��ȡ��ǰ����ʾ֡��Χ
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
			//�������ʱ�����֡������¶���֡
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
	else//�� �� �� ��
	{
		//�����泯��ǰ������ʾͣ�µ�֡(��������˷����˳��Ĺ�ϵ����animeIndex[0][0]Ϊ���ϵ�֡�����б�ţ�[1][0]Ϊ����,�Դ����ƣ�����Ϊ������
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
	//�������
	int left = (int)(x + position[0] - ScrollX);
	//�Ҳ�����
	int right = (int)(left + width*scaling);
	//�߶�
	int down = (int)(y + position[1] - ScrollY);
	int up = down - (int)(height*scaling*0.15f);
	RECT playerMP{ left,up,right,down };
	//���Ƶײ��ĺ�ɫ
	RECT playerHP{ left,(LONG)(up - height*scaling*0.15 - 1),right,up - 1 };
	d3ddev->ColorFill(backbuffer, &playerHP, D3DCOLOR_XRGB(0, 0, 0));
	d3ddev->ColorFill(backbuffer, &playerMP, D3DCOLOR_XRGB(0, 0, 0));
	//����Ѫ��/��
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
	//����vector��Ԫ�ص����ã������µ�ջ���������غ��Ұָ��
	//for each (auto var in constantNPCs) {}
	//���ص��ǵ��������ͣ�����Ϊ��
	//for (auto var = constantNPCs.begin(); var < constantNPCs.end(); var++){}
	//����vector��Ԫ�ص����ã��µ�ջ����Ҳ�Ƿ���Ұָ��
	//for(auto var:constantNPCs){	}
	//C++11�º�������һЩcollection(vector,list,set,map)��Ԫ��ִ��function()
	//for_each(iterator first, iterator last, function());
	//����֮����ԭʼ�Ĳ���
	for (int i = 0; i<(int)constantNPCs.size(); i++)
	{
		//�������ò���Ҫ�����õ�����
		CHARACTOR& var = constantNPCs[i];
		//�����Լ����Լ����Լ��ľ�����Զ��0��
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