#ifndef G_PLATFORM_H
#define G_PLATFORM_H

#include <stdint.h>
#include <hal_device.h>


enum DEVICE_STATUS_e {
	DEVICE_STATUS_POWER_OFF = 0x01, 
	DEVICE_STATUS_POWER_SCANNING = 0x02,
	DEVICE_STATUS_POWER_SCAN_OVER = 0x03
};

typedef struct
{
	uint8_t  Second;
	uint8_t  Minute;
	uint8_t  Hour;
	uint8_t  Day;
	uint8_t  Week;
	uint8_t  Month;
	uint32_t Year;
}RtcStruct;

typedef struct
{
	double    Longitude;                //地理位置经度
	double    Latitude;                 //地理位置纬度
	double    Altitude;                 //海拔高度
//	float    Longitude;                //地理位置经度
//	float    Latitude;                 //地理位置纬度
//	float    Altitude;                 //海拔高度

	uint32_t SerialNumber;              //SN
	uint32_t PD;
	uint32_t Version;                   //版本
	uint8_t  SendPeriod;                //数据发送周期
	uint32_t DeviceID;                  //设备编号
	uint8_t  DeviceType;                //设备类型
//	uint8_t  CommunicationIndex;		//通信方式
	uint8_t  PowerQuantity;             //电量
	float    BATVoltage;                //电池电压   //2.75-4.2V
	uint16_t LoraDeveui;				//LoRa Deveui

	uint16_t SensorStatus;              //传感器状态
	char DeviceStatus;
	char ProductKey[32];
	char DeviceName[32];
	char DeviceSecret[64];
	uint8_t hmacmd5Val[64];
	uint8_t hmacsha1[64];
	uint8_t AliiotToken[64];
	uint8_t AliiotID[64];
}TerminalPlatform;

typedef struct
{
	uint16_t SeqNumber;          //发送lora/NB数据的序号       每发送一次增加1，每24小时清0一次

	char 	 *nIP;				 //如涉及IP地址
	char 	 *nPort;			 //如涉及Port
	uint8_t  GPRSNet;            //GPRS网络信号状态
	uint8_t  GPRSAttached;       //GPRS网络附着状态
	uint8_t  GPRSConnect;        //GPRS网络TCP连接状态
	char GPRSStatus;
	char GPRSATStatus;
	uint32_t Http_Cid;
	uint8_t  LoRaJoinNET;        //LoRa入网状态

	float GPSLng_Point;          //GPS经度
	float GPSLat_Point;          //GPS纬度
}TransMethodPlatform;


typedef struct
{
	float      VOC;			         //VOC       0~30.00   ppm
	float      CO;		             //CO        0~200.0   ppm
	uint32_t   CO2;		             //CO2       0~5000    ppm
}VOCPlatform;//VOC检测平台

typedef struct
{
	float Temperature;		     //室外温度         -40~60.0       ℃
	float Humidity;				 //室外湿度          0~100.0    %
	float WindSpeed;			 //风速                 0~30.0     m/s
	int   WindDirection;		 //风向                 0~360      °
	uint16_t PM25;			     //PM2.5     0~6000     ug/m3
	uint16_t PM10;			     //PM10      0~6000     ug/m3
	float Noise;                 //噪音                 30.0~120.0 dB
}DustPlatform;//扬尘噪音监测站

typedef struct
{
	float WindSpeed;			 //风速                 0~30.0    m/s
	int   WindDirection;		 //风向                 0~360     °
	float Temperature;		     //室外温度         -40~60.0      ℃
	float Humidity;				 //室外湿度          0~100.0   %
	float PM25;			         //PM2.5     0~6000    ug/m3
	float PM10;			      	 //PM10      0~6000    ug/m3
	uint32_t Illumination;       //光照                 0~200000  Lux
	float AirPressure;           //大气压
	float RainGauge;             //雨量                  0~4.0     mm/min
	float Radiation;             //总辐射              0~2000    W/m2
}MeteorologyPlatform;//气象检测平台

typedef struct
{
	float RainGauge;             //雨量                  0~4.0     mm/min
	uint16_t LVValue;            //液位
}WRainPlatform;       //水雨情监测平台

typedef struct
{
	float CODValue;
	int16_t ORPValue;
	float TemValue;
	float DoPercent;
	float DOValue;
	float DORealValue;
	float NH4Value;
	float ZSValue;
	float PHValue;
	uint16_t ECValue;
	uint16_t LVValue;
	float CHLValue; //叶绿素
}WaterPlatform;       //水质监测平台

typedef struct
{
	float SoilTemp;				 //土壤温度                -40~80.00   ℃
	float SoilHum;		         //土壤水分（湿度）   0~80.00   %
	uint16_t SoilConductivity;	 //土壤电导率              0~20000   us/cm
	float SoilPH;			     //土壤PH        0~14.00   PH
}SoilPlatform;   //土壤检测平台

typedef struct
{
	float Temperature;		     //室外温度                 -40~60.0     ℃
	float Humidity;				 //室外湿度                  0~100.0   %
	uint32_t Illumination;       //光照                         0~200000  Lux
	float WaterPH;				 //水质PH        0~14.00   PH
	float WaterPHTemp;	         //水质PH传感器温度  0~70.0         ℃
	float WaterTemp;		     //水温                        -40~70.0      ℃
	float SoilTemp;				 //土壤温度                -40~80.00    ℃
	float SoilHum;		         //土壤水分（湿度）   0~50.00   %
	uint16_t SoilConductivity;	 //土壤电导率              0~20000   us/cm
	float SoilPH;			     //土壤PH        0~14.00   PH
}AgriculturePlatform;   //农业检测平台

typedef struct
{
	float Depth1Vlaue; //液位1
	float Speed1Vlaue; //流速1
	float Depth2Vlaue; //液位2
	float Speed2Vlaue; //流速2
	float Depth3Vlaue; //流速3
	float Speed3Vlaue; //液位3
}FlowmeterPlatform;    //流量计监测平台

typedef struct
{
	uint16_t LVValue;      //液位
}SeeperPlatform;    //易涝点液位监测平台

typedef struct
{
//	double Depth1Vlaue; //液位
//	double Speed1Vlaue; //流速
	float Depth1Vlaue;  //液位
	float Speed1Vlaue;  //流速
}FlowPlatform;    //窨井管道流量监测平台

typedef struct
{
	float RainGauge;             //雨量                  0~4.0     mm/min
}RainPlatform;    //雨量监测平台

typedef struct
{
	float UltrasonicLevel;
	float InputmodeLevel;
	float RealLevel;
}PitWellPlatform;     //一体化窨井液位监测平台

typedef struct
{
	float InputmodeLevel;
}InputmodeWellPlatform;    //投入式窨井液位监测平台

typedef struct
{
	float Temperature;		     //室外温度         -40~60.0      ℃
	float Humidity;				 //室外湿度          0~100.0   %
	uint32_t Illumination;       //光照                        0~200000  Lux
	float H2S;			         //H2S       0~30.0   ppm
	float NH3;		             //NH3       0~30.0   ppm
}NoxiousGasPlatform;      //有害气体监测平台

typedef struct
{
	float Temperature;		     //室外温度                 -40~60.0      ℃
	float Humidity;				 //室外湿度                 0~100.0   %
	uint32_t Illumination;       //光照                        0~200000  Lux
	float SoilTemp;				 //土壤温度                -20~80.00    ℃
	float SoilHum;		         //土壤水分（湿度）   0~100.00   %
	uint16_t SoilConductivity;	 //土壤电导率              0~20000   us/cm
	uint16_t CO2;		         //CO2          0~5000    ppm
}WetherSoilPlatform;      //气象土壤监测平台

typedef struct
{
	float Temperature;		     //室外温度                 -40~60.0      ℃
	float Humidity;				 //室外湿度                 0~100.0   %
	uint32_t Illumination;       //光照                        0~200000  Lux
	uint16_t CO2;		         //CO2          0~5000    ppm
	float LeafHumidity;		     //叶面湿度                 0~100.0   %
	float SoilTemp;				 //土壤温度                -20~80.00    ℃
	float SoilHum;		         //土壤水分（湿度）   0~100.00   %
	uint16_t SoilConductivity;	 //土壤电导率              0~20000   us/cm
}PlantingPlatform;         //农作种植环境监测平台

typedef struct
{
	uint16_t Level;		        //雷达液位                     0.05m～30.00m； 5cm-3000cm，分辨率2mm
	float Flowrate;		        //雷达流速                     0~15000    mm/s
}LevelFlowratePlatform;         //雷达液位流速监测仪

typedef struct
{
	float Temperature;		     //空气温度          0~99.0         ℃
	float Humidity;				 //空气湿度          0~99.0    %
	uint16_t PM25;			     //PM2.5     0~500     ug/m3
	uint16_t PM10;			     //PM10      0~500     ug/m3
	float CO;			         //CO        0~20.00   ppm
	float SO2;		             //SO2       0~10.000  ppm
	float NO2;			         //NO2       0~10.000  ppm
	float O3;		             //O3        0~10.000  ppm
}AliveNestPlatform;             //一体化空气监测仪

typedef struct
{
	float Temperature;		     //水温          0~50.00         ℃
	float CL;		             //氯离子      0~10000.00    ℃    正常在200以下
}CLCupboardPlatform;             //氯离子柜式监测站

typedef struct
{
    TerminalPlatform           TerminalInfoData;
    TransMethodPlatform        TransMethodData;

#if (PRODUCT_TYPE == Voc_Station) 
    VOCPlatform                VOCData;
#elif (PRODUCT_TYPE == Dust_Station) 
    DustPlatform               DustData;
#elif (PRODUCT_TYPE == WRain_Station) 
    WRainPlatform              WRainData;
#endif
    MeteorologyPlatform        MeteorologyData;
	WaterPlatform			   WaterData;
#if (PRODUCT_TYPE == Soil_Station) 
	SoilPlatform               SoilData;
#elif (PRODUCT_TYPE == Agriculture_Station) 
	AgriculturePlatform        AgricultureData;
#elif (PRODUCT_TYPE == Flowmeter_Station) 
	FlowmeterPlatform		   FlowmeterData;
#elif (PRODUCT_TYPE == Seeper_Station) 
	SeeperPlatform             SeeperData;
#elif (PRODUCT_TYPE == Flow_Station) 
	FlowPlatform               FlowData;
#elif (PRODUCT_TYPE == Rain_Station)    
	RainPlatform               RainData;
#endif
	PitWellPlatform            PitWellData;
	InputmodeWellPlatform      InputmodeWellData;

	NoxiousGasPlatform         NoxiousGasData;
	WetherSoilPlatform         WetherSoilData;

	PlantingPlatform           PlantingData;
	LevelFlowratePlatform      LevelFlowrateData;

	AliveNestPlatform          AliveNestData;
	CLCupboardPlatform         CLCupboardData;

//  以下为系统保留数据，系统层直接使用
    RtcStruct Rtc;


}DataStruct;

typedef struct
{
    DataStruct Data;
}AppStruct;






extern AppStruct  App;
extern DataStruct *AppDataPointer;  

extern uint32_t SensorCahe;
extern uint32_t sSensorCahe;

void InqureSensor(void);
char *MakeJsonBodyData(DataStruct *DataPointer);
void ScadaData_base_Init();
void Terminal_Para_Init(void);

#endif


