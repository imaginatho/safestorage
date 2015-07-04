#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include <crc32.h>

uint32_t _crc32[8][256] = {
    {0x00000000,0x04C11DB7,0x09823B6E,0x0D4326D9,0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005
    ,0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD
    ,0x4C11DB70,0x48D0C6C7,0x4593E01E,0x4152FDA9,0x5F15ADAC,0x5BD4B01B,0x569796C2,0x52568B75
    ,0x6A1936C8,0x6ED82B7F,0x639B0DA6,0x675A1011,0x791D4014,0x7DDC5DA3,0x709F7B7A,0x745E66CD
    ,0x9823B6E0,0x9CE2AB57,0x91A18D8E,0x95609039,0x8B27C03C,0x8FE6DD8B,0x82A5FB52,0x8664E6E5
    ,0xBE2B5B58,0xBAEA46EF,0xB7A96036,0xB3687D81,0xAD2F2D84,0xA9EE3033,0xA4AD16EA,0xA06C0B5D
    ,0xD4326D90,0xD0F37027,0xDDB056FE,0xD9714B49,0xC7361B4C,0xC3F706FB,0xCEB42022,0xCA753D95
    ,0xF23A8028,0xF6FB9D9F,0xFBB8BB46,0xFF79A6F1,0xE13EF6F4,0xE5FFEB43,0xE8BCCD9A,0xEC7DD02D
    ,0x34867077,0x30476DC0,0x3D044B19,0x39C556AE,0x278206AB,0x23431B1C,0x2E003DC5,0x2AC12072
    ,0x128E9DCF,0x164F8078,0x1B0CA6A1,0x1FCDBB16,0x018AEB13,0x054BF6A4,0x0808D07D,0x0CC9CDCA
    ,0x7897AB07,0x7C56B6B0,0x71159069,0x75D48DDE,0x6B93DDDB,0x6F52C06C,0x6211E6B5,0x66D0FB02
    ,0x5E9F46BF,0x5A5E5B08,0x571D7DD1,0x53DC6066,0x4D9B3063,0x495A2DD4,0x44190B0D,0x40D816BA
    ,0xACA5C697,0xA864DB20,0xA527FDF9,0xA1E6E04E,0xBFA1B04B,0xBB60ADFC,0xB6238B25,0xB2E29692
    ,0x8AAD2B2F,0x8E6C3698,0x832F1041,0x87EE0DF6,0x99A95DF3,0x9D684044,0x902B669D,0x94EA7B2A
    ,0xE0B41DE7,0xE4750050,0xE9362689,0xEDF73B3E,0xF3B06B3B,0xF771768C,0xFA325055,0xFEF34DE2
    ,0xC6BCF05F,0xC27DEDE8,0xCF3ECB31,0xCBFFD686,0xD5B88683,0xD1799B34,0xDC3ABDED,0xD8FBA05A
    ,0x690CE0EE,0x6DCDFD59,0x608EDB80,0x644FC637,0x7A089632,0x7EC98B85,0x738AAD5C,0x774BB0EB
    ,0x4F040D56,0x4BC510E1,0x46863638,0x42472B8F,0x5C007B8A,0x58C1663D,0x558240E4,0x51435D53
    ,0x251D3B9E,0x21DC2629,0x2C9F00F0,0x285E1D47,0x36194D42,0x32D850F5,0x3F9B762C,0x3B5A6B9B
    ,0x0315D626,0x07D4CB91,0x0A97ED48,0x0E56F0FF,0x1011A0FA,0x14D0BD4D,0x19939B94,0x1D528623
    ,0xF12F560E,0xF5EE4BB9,0xF8AD6D60,0xFC6C70D7,0xE22B20D2,0xE6EA3D65,0xEBA91BBC,0xEF68060B
    ,0xD727BBB6,0xD3E6A601,0xDEA580D8,0xDA649D6F,0xC423CD6A,0xC0E2D0DD,0xCDA1F604,0xC960EBB3
    ,0xBD3E8D7E,0xB9FF90C9,0xB4BCB610,0xB07DABA7,0xAE3AFBA2,0xAAFBE615,0xA7B8C0CC,0xA379DD7B
    ,0x9B3660C6,0x9FF77D71,0x92B45BA8,0x9675461F,0x8832161A,0x8CF30BAD,0x81B02D74,0x857130C3
    ,0x5D8A9099,0x594B8D2E,0x5408ABF7,0x50C9B640,0x4E8EE645,0x4A4FFBF2,0x470CDD2B,0x43CDC09C
    ,0x7B827D21,0x7F436096,0x7200464F,0x76C15BF8,0x68860BFD,0x6C47164A,0x61043093,0x65C52D24
    ,0x119B4BE9,0x155A565E,0x18197087,0x1CD86D30,0x029F3D35,0x065E2082,0x0B1D065B,0x0FDC1BEC
    ,0x3793A651,0x3352BBE6,0x3E119D3F,0x3AD08088,0x2497D08D,0x2056CD3A,0x2D15EBE3,0x29D4F654
    ,0xC5A92679,0xC1683BCE,0xCC2B1D17,0xC8EA00A0,0xD6AD50A5,0xD26C4D12,0xDF2F6BCB,0xDBEE767C
    ,0xE3A1CBC1,0xE760D676,0xEA23F0AF,0xEEE2ED18,0xF0A5BD1D,0xF464A0AA,0xF9278673,0xFDE69BC4
    ,0x89B8FD09,0x8D79E0BE,0x803AC667,0x84FBDBD0,0x9ABC8BD5,0x9E7D9662,0x933EB0BB,0x97FFAD0C
    ,0xAFB010B1,0xAB710D06,0xA6322BDF,0xA2F33668,0xBCB4666D,0xB8757BDA,0xB5365D03,0xB1F740B4},
    {0x00000000,0xA37D1C66,0x9022E4A6,0x335FF8C0,0x2484D4FB,0x87F9C89D,0xB4A6305D,0x17DB2C3B
    ,0x9B10682B,0x386D744D,0x0B328C8D,0xA84F90EB,0xBF94BCD0,0x1CE9A0B6,0x2FB65876,0x8CCB4410
    ,0xE0F80C3C,0x4385105A,0x70DAE89A,0xD3A7F4FC,0xC47CD8C7,0x6701C4A1,0x545E3C61,0xF7232007
    ,0x7BE86417,0xD8957871,0xEBCA80B1,0x48B79CD7,0x5F6CB0EC,0xFC11AC8A,0xCF4E544A,0x6C33482C
    ,0xC53105CF,0x664C19A9,0x5513E169,0xF66EFD0F,0xE1B5D134,0x42C8CD52,0x71973592,0xD2EA29F4
    ,0x5E216DE4,0xFD5C7182,0xCE038942,0x6D7E9524,0x7AA5B91F,0xD9D8A579,0xEA875DB9,0x49FA41DF
    ,0x25C909F3,0x86B41595,0xB5EBED55,0x1696F133,0x014DDD08,0xA230C16E,0x916F39AE,0x321225C8
    ,0xBED961D8,0x1DA47DBE,0x2EFB857E,0x8D869918,0x9A5DB523,0x3920A945,0x0A7F5185,0xA9024DE3
    ,0xFEC7CB92,0x5DBAD7F4,0x6EE52F34,0xCD983352,0xDA431F69,0x793E030F,0x4A61FBCF,0xE91CE7A9
    ,0x65D7A3B9,0xC6AABFDF,0xF5F5471F,0x56885B79,0x41537742,0xE22E6B24,0xD17193E4,0x720C8F82
    ,0x1E3FC7AE,0xBD42DBC8,0x8E1D2308,0x2D603F6E,0x3ABB1355,0x99C60F33,0xAA99F7F3,0x09E4EB95
    ,0x852FAF85,0x2652B3E3,0x150D4B23,0xB6705745,0xA1AB7B7E,0x02D66718,0x31899FD8,0x92F483BE
    ,0x3BF6CE5D,0x988BD23B,0xABD42AFB,0x08A9369D,0x1F721AA6,0xBC0F06C0,0x8F50FE00,0x2C2DE266
    ,0xA0E6A676,0x039BBA10,0x30C442D0,0x93B95EB6,0x8462728D,0x271F6EEB,0x1440962B,0xB73D8A4D
    ,0xDB0EC261,0x7873DE07,0x4B2C26C7,0xE8513AA1,0xFF8A169A,0x5CF70AFC,0x6FA8F23C,0xCCD5EE5A
    ,0x401EAA4A,0xE363B62C,0xD03C4EEC,0x7341528A,0x649A7EB1,0xC7E762D7,0xF4B89A17,0x57C58671
    ,0xF94E8A93,0x5A3396F5,0x696C6E35,0xCA117253,0xDDCA5E68,0x7EB7420E,0x4DE8BACE,0xEE95A6A8
    ,0x625EE2B8,0xC123FEDE,0xF27C061E,0x51011A78,0x46DA3643,0xE5A72A25,0xD6F8D2E5,0x7585CE83
    ,0x19B686AF,0xBACB9AC9,0x89946209,0x2AE97E6F,0x3D325254,0x9E4F4E32,0xAD10B6F2,0x0E6DAA94
    ,0x82A6EE84,0x21DBF2E2,0x12840A22,0xB1F91644,0xA6223A7F,0x055F2619,0x3600DED9,0x957DC2BF
    ,0x3C7F8F5C,0x9F02933A,0xAC5D6BFA,0x0F20779C,0x18FB5BA7,0xBB8647C1,0x88D9BF01,0x2BA4A367
    ,0xA76FE777,0x0412FB11,0x374D03D1,0x94301FB7,0x83EB338C,0x20962FEA,0x13C9D72A,0xB0B4CB4C
    ,0xDC878360,0x7FFA9F06,0x4CA567C6,0xEFD87BA0,0xF803579B,0x5B7E4BFD,0x6821B33D,0xCB5CAF5B
    ,0x4797EB4B,0xE4EAF72D,0xD7B50FED,0x74C8138B,0x63133FB0,0xC06E23D6,0xF331DB16,0x504CC770
    ,0x07894101,0xA4F45D67,0x97ABA5A7,0x34D6B9C1,0x230D95FA,0x8070899C,0xB32F715C,0x10526D3A
    ,0x9C99292A,0x3FE4354C,0x0CBBCD8C,0xAFC6D1EA,0xB81DFDD1,0x1B60E1B7,0x283F1977,0x8B420511
    ,0xE7714D3D,0x440C515B,0x7753A99B,0xD42EB5FD,0xC3F599C6,0x608885A0,0x53D77D60,0xF0AA6106
    ,0x7C612516,0xDF1C3970,0xEC43C1B0,0x4F3EDDD6,0x58E5F1ED,0xFB98ED8B,0xC8C7154B,0x6BBA092D
    ,0xC2B844CE,0x61C558A8,0x529AA068,0xF1E7BC0E,0xE63C9035,0x45418C53,0x761E7493,0xD56368F5
    ,0x59A82CE5,0xFAD53083,0xC98AC843,0x6AF7D425,0x7D2CF81E,0xDE51E478,0xED0E1CB8,0x4E7300DE
    ,0x224048F2,0x813D5494,0xB262AC54,0x111FB032,0x06C49C09,0xA5B9806F,0x96E678AF,0x359B64C9
    ,0xB95020D9,0x1A2D3CBF,0x2972C47F,0x8A0FD819,0x9DD4F422,0x3EA9E844,0x0DF61084,0xAE8B0CE2},
    {0x00000000,0xB680F639,0xEB393958,0x5DB9CF61,0xA2D7B2BC,0x14574485,0x49EE8BE4,0xFF6E7DDD
    ,0xB3F36DE9,0x05739BD0,0x58CA54B1,0xEE4AA288,0x1124DF55,0xA7A4296C,0xFA1DE60D,0x4C9D1034
    ,0xE1DE0EF8,0x575EF8C1,0x0AE737A0,0xBC67C199,0x4309BC44,0xF5894A7D,0xA830851C,0x1EB07325
    ,0x522D6311,0xE4AD9528,0xB9145A49,0x0F94AC70,0xF0FAD1AD,0x467A2794,0x1BC3E8F5,0xAD431ECC
    ,0x65001C21,0xD380EA18,0x8E392579,0x38B9D340,0xC7D7AE9D,0x715758A4,0x2CEE97C5,0x9A6E61FC
    ,0xD6F371C8,0x607387F1,0x3DCA4890,0x8B4ABEA9,0x7424C374,0xC2A4354D,0x9F1DFA2C,0x299D0C15
    ,0x84DE12D9,0x325EE4E0,0x6FE72B81,0xD967DDB8,0x2609A065,0x9089565C,0xCD30993D,0x7BB06F04
    ,0x372D7F30,0x81AD8909,0xDC144668,0x6A94B051,0x95FACD8C,0x237A3BB5,0x7EC3F4D4,0xC84302ED
    ,0x2C61C73B,0x9AE13102,0xC758FE63,0x71D8085A,0x8EB67587,0x383683BE,0x658F4CDF,0xD30FBAE6
    ,0x9F92AAD2,0x29125CEB,0x74AB938A,0xC22B65B3,0x3D45186E,0x8BC5EE57,0xD67C2136,0x60FCD70F
    ,0xCDBFC9C3,0x7B3F3FFA,0x2686F09B,0x900606A2,0x6F687B7F,0xD9E88D46,0x84514227,0x32D1B41E
    ,0x7E4CA42A,0xC8CC5213,0x95759D72,0x23F56B4B,0xDC9B1696,0x6A1BE0AF,0x37A22FCE,0x8122D9F7
    ,0x4961DB1A,0xFFE12D23,0xA258E242,0x14D8147B,0xEBB669A6,0x5D369F9F,0x008F50FE,0xB60FA6C7
    ,0xFA92B6F3,0x4C1240CA,0x11AB8FAB,0xA72B7992,0x5845044F,0xEEC5F276,0xB37C3D17,0x05FCCB2E
    ,0xA8BFD5E2,0x1E3F23DB,0x4386ECBA,0xF5061A83,0x0A68675E,0xBCE89167,0xE1515E06,0x57D1A83F
    ,0x1B4CB80B,0xADCC4E32,0xF0758153,0x46F5776A,0xB99B0AB7,0x0F1BFC8E,0x52A233EF,0xE422C5D6
    ,0x28A753CD,0x9E27A5F4,0xC39E6A95,0x751E9CAC,0x8A70E171,0x3CF01748,0x6149D829,0xD7C92E10
    ,0x9B543E24,0x2DD4C81D,0x706D077C,0xC6EDF145,0x39838C98,0x8F037AA1,0xD2BAB5C0,0x643A43F9
    ,0xC9795D35,0x7FF9AB0C,0x2240646D,0x94C09254,0x6BAEEF89,0xDD2E19B0,0x8097D6D1,0x361720E8
    ,0x7A8A30DC,0xCC0AC6E5,0x91B30984,0x2733FFBD,0xD85D8260,0x6EDD7459,0x3364BB38,0x85E44D01
    ,0x4DA74FEC,0xFB27B9D5,0xA69E76B4,0x101E808D,0xEF70FD50,0x59F00B69,0x0449C408,0xB2C93231
    ,0xFE542205,0x48D4D43C,0x156D1B5D,0xA3EDED64,0x5C8390B9,0xEA036680,0xB7BAA9E1,0x013A5FD8
    ,0xAC794114,0x1AF9B72D,0x4740784C,0xF1C08E75,0x0EAEF3A8,0xB82E0591,0xE597CAF0,0x53173CC9
    ,0x1F8A2CFD,0xA90ADAC4,0xF4B315A5,0x4233E39C,0xBD5D9E41,0x0BDD6878,0x5664A719,0xE0E45120
    ,0x04C694F6,0xB24662CF,0xEFFFADAE,0x597F5B97,0xA611264A,0x1091D073,0x4D281F12,0xFBA8E92B
    ,0xB735F91F,0x01B50F26,0x5C0CC047,0xEA8C367E,0x15E24BA3,0xA362BD9A,0xFEDB72FB,0x485B84C2
    ,0xE5189A0E,0x53986C37,0x0E21A356,0xB8A1556F,0x47CF28B2,0xF14FDE8B,0xACF611EA,0x1A76E7D3
    ,0x56EBF7E7,0xE06B01DE,0xBDD2CEBF,0x0B523886,0xF43C455B,0x42BCB362,0x1F057C03,0xA9858A3A
    ,0x61C688D7,0xD7467EEE,0x8AFFB18F,0x3C7F47B6,0xC3113A6B,0x7591CC52,0x28280333,0x9EA8F50A
    ,0xD235E53E,0x64B51307,0x390CDC66,0x8F8C2A5F,0x70E25782,0xC662A1BB,0x9BDB6EDA,0x2D5B98E3
    ,0x8018862F,0x36987016,0x6B21BF77,0xDDA1494E,0x22CF3493,0x944FC2AA,0xC9F60DCB,0x7F76FBF2
    ,0x33EBEBC6,0x856B1DFF,0xD8D2D29E,0x6E5224A7,0x913C597A,0x27BCAF43,0x7A056022,0xCC85961B},
    {0x00000000,0xF64D1D69,0x5E747F86,0xA83962EF,0x8890C1A8,0x7EDDDCC1,0xD6E4BE2E,0x20A9A347
    ,0xE7D3251B,0x119E3872,0xB9A75A9D,0x4FEA47F4,0x6F43E4B3,0x990EF9DA,0x31379B35,0xC77A865C
    ,0xAF51CEBF,0x591CD3D6,0xF125B139,0x0768AC50,0x27C10F17,0xD18C127E,0x79B57091,0x8FF86DF8
    ,0x4882EBA4,0xBECFF6CD,0x16F69422,0xE0BB894B,0xC0122A0C,0x365F3765,0x9E66558A,0x682B48E3
    ,0x9C87AB4B,0x6ACAB622,0xC2F3D4CD,0x34BEC9A4,0x14176AE3,0xE25A778A,0x4A631565,0xBC2E080C
    ,0x7B548E50,0x8D199339,0x2520F1D6,0xD36DECBF,0xF3C44FF8,0x05895291,0xADB0307E,0x5BFD2D17
    ,0x33D665F4,0xC59B789D,0x6DA21A72,0x9BEF071B,0xBB46A45C,0x4D0BB935,0xE532DBDA,0x137FC6B3
    ,0xD40540EF,0x22485D86,0x8A713F69,0x7C3C2200,0x5C958147,0xAAD89C2E,0x02E1FEC1,0xF4ACE3A8
    ,0xFF55C736,0x0918DA5F,0xA121B8B0,0x576CA5D9,0x77C5069E,0x81881BF7,0x29B17918,0xDFFC6471
    ,0x1886E22D,0xEECBFF44,0x46F29DAB,0xB0BF80C2,0x90162385,0x665B3EEC,0xCE625C03,0x382F416A
    ,0x50040989,0xA64914E0,0x0E70760F,0xF83D6B66,0xD894C821,0x2ED9D548,0x86E0B7A7,0x70ADAACE
    ,0xB7D72C92,0x419A31FB,0xE9A35314,0x1FEE4E7D,0x3F47ED3A,0xC90AF053,0x613392BC,0x977E8FD5
    ,0x63D26C7D,0x959F7114,0x3DA613FB,0xCBEB0E92,0xEB42ADD5,0x1D0FB0BC,0xB536D253,0x437BCF3A
    ,0x84014966,0x724C540F,0xDA7536E0,0x2C382B89,0x0C9188CE,0xFADC95A7,0x52E5F748,0xA4A8EA21
    ,0xCC83A2C2,0x3ACEBFAB,0x92F7DD44,0x64BAC02D,0x4413636A,0xB25E7E03,0x1A671CEC,0xEC2A0185
    ,0x2B5087D9,0xDD1D9AB0,0x7524F85F,0x8369E536,0xA3C04671,0x558D5B18,0xFDB439F7,0x0BF9249E
    ,0x6C6FB119,0x9A22AC70,0x321BCE9F,0xC456D3F6,0xE4FF70B1,0x12B26DD8,0xBA8B0F37,0x4CC6125E
    ,0x8BBC9402,0x7DF1896B,0xD5C8EB84,0x2385F6ED,0x032C55AA,0xF56148C3,0x5D582A2C,0xAB153745
    ,0xC33E7FA6,0x357362CF,0x9D4A0020,0x6B071D49,0x4BAEBE0E,0xBDE3A367,0x15DAC188,0xE397DCE1
    ,0x24ED5ABD,0xD2A047D4,0x7A99253B,0x8CD43852,0xAC7D9B15,0x5A30867C,0xF209E493,0x0444F9FA
    ,0xF0E81A52,0x06A5073B,0xAE9C65D4,0x58D178BD,0x7878DBFA,0x8E35C693,0x260CA47C,0xD041B915
    ,0x173B3F49,0xE1762220,0x494F40CF,0xBF025DA6,0x9FABFEE1,0x69E6E388,0xC1DF8167,0x37929C0E
    ,0x5FB9D4ED,0xA9F4C984,0x01CDAB6B,0xF780B602,0xD7291545,0x2164082C,0x895D6AC3,0x7F1077AA
    ,0xB86AF1F6,0x4E27EC9F,0xE61E8E70,0x10539319,0x30FA305E,0xC6B72D37,0x6E8E4FD8,0x98C352B1
    ,0x933A762F,0x65776B46,0xCD4E09A9,0x3B0314C0,0x1BAAB787,0xEDE7AAEE,0x45DEC801,0xB393D568
    ,0x74E95334,0x82A44E5D,0x2A9D2CB2,0xDCD031DB,0xFC79929C,0x0A348FF5,0xA20DED1A,0x5440F073
    ,0x3C6BB890,0xCA26A5F9,0x621FC716,0x9452DA7F,0xB4FB7938,0x42B66451,0xEA8F06BE,0x1CC21BD7
    ,0xDBB89D8B,0x2DF580E2,0x85CCE20D,0x7381FF64,0x53285C23,0xA565414A,0x0D5C23A5,0xFB113ECC
    ,0x0FBDDD64,0xF9F0C00D,0x51C9A2E2,0xA784BF8B,0x872D1CCC,0x716001A5,0xD959634A,0x2F147E23
    ,0xE86EF87F,0x1E23E516,0xB61A87F9,0x40579A90,0x60FE39D7,0x96B324BE,0x3E8A4651,0xC8C75B38
    ,0xA0EC13DB,0x56A10EB2,0xFE986C5D,0x08D57134,0x287CD273,0xDE31CF1A,0x7608ADF5,0x8045B09C
    ,0x473F36C0,0xB1722BA9,0x194B4946,0xEF06542F,0xCFAFF768,0x39E2EA01,0x91DB88EE,0x67969587},
    {0x00000000,0x8E9A7B85,0x73D4D923,0xFD4EA2A6,0xD7AF2B77,0x593550F2,0xA47BF254,0x2AE189D1
    ,0x67BDC334,0xE927B8B1,0x14691A17,0x9AF36192,0xB012E843,0x3E8893C6,0xC3C63160,0x4D5C4AE5
    ,0x85DE610D,0x0B441A88,0xF60AB82E,0x7890C3AB,0x52714A7A,0xDCEB31FF,0x21A59359,0xAF3FE8DC
    ,0xE263A239,0x6CF9D9BC,0x91B77B1A,0x1F2D009F,0x35CC894E,0xBB56F2CB,0x4618506D,0xC8822BE8
    ,0x1F513CBD,0x91CB4738,0x6C85E59E,0xE21F9E1B,0xC8FE17CA,0x46646C4F,0xBB2ACEE9,0x35B0B56C
    ,0x78ECFF89,0xF676840C,0x0B3826AA,0x85A25D2F,0xAF43D4FE,0x21D9AF7B,0xDC970DDD,0x520D7658
    ,0x9A8F5DB0,0x14152635,0xE95B8493,0x67C1FF16,0x4D2076C7,0xC3BA0D42,0x3EF4AFE4,0xB06ED461
    ,0xFD329E84,0x73A8E501,0x8EE647A7,0x007C3C22,0x2A9DB5F3,0xA407CE76,0x59496CD0,0xD7D31755
    ,0xCE4B75E5,0x40D10E60,0xBD9FACC6,0x3305D743,0x19E45E92,0x977E2517,0x6A3087B1,0xE4AAFC34
    ,0xA9F6B6D1,0x276CCD54,0xDA226FF2,0x54B81477,0x7E599DA6,0xF0C3E623,0x0D8D4485,0x83173F00
    ,0x4B9514E8,0xC50F6F6D,0x3841CDCB,0xB6DBB64E,0x9C3A3F9F,0x12A0441A,0xEFEEE6BC,0x61749D39
    ,0x2C28D7DC,0xA2B2AC59,0x5FFC0EFF,0xD166757A,0xFB87FCAB,0x751D872E,0x88532588,0x06C95E0D
    ,0xD11A4958,0x5F8032DD,0xA2CE907B,0x2C54EBFE,0x06B5622F,0x882F19AA,0x7561BB0C,0xFBFBC089
    ,0xB6A78A6C,0x383DF1E9,0xC573534F,0x4BE928CA,0x6108A11B,0xEF92DA9E,0x12DC7838,0x9C4603BD
    ,0x54C42855,0xDA5E53D0,0x2710F176,0xA98A8AF3,0x836B0322,0x0DF178A7,0xF0BFDA01,0x7E25A184
    ,0x3379EB61,0xBDE390E4,0x40AD3242,0xCE3749C7,0xE4D6C016,0x6A4CBB93,0x97021935,0x199862B0
    ,0x6EB444CE,0xE02E3F4B,0x1D609DED,0x93FAE668,0xB91B6FB9,0x3781143C,0xCACFB69A,0x4455CD1F
    ,0x090987FA,0x8793FC7F,0x7ADD5ED9,0xF447255C,0xDEA6AC8D,0x503CD708,0xAD7275AE,0x23E80E2B
    ,0xEB6A25C3,0x65F05E46,0x98BEFCE0,0x16248765,0x3CC50EB4,0xB25F7531,0x4F11D797,0xC18BAC12
    ,0x8CD7E6F7,0x024D9D72,0xFF033FD4,0x71994451,0x5B78CD80,0xD5E2B605,0x28AC14A3,0xA6366F26
    ,0x71E57873,0xFF7F03F6,0x0231A150,0x8CABDAD5,0xA64A5304,0x28D02881,0xD59E8A27,0x5B04F1A2
    ,0x1658BB47,0x98C2C0C2,0x658C6264,0xEB1619E1,0xC1F79030,0x4F6DEBB5,0xB2234913,0x3CB93296
    ,0xF43B197E,0x7AA162FB,0x87EFC05D,0x0975BBD8,0x23943209,0xAD0E498C,0x5040EB2A,0xDEDA90AF
    ,0x9386DA4A,0x1D1CA1CF,0xE0520369,0x6EC878EC,0x4429F13D,0xCAB38AB8,0x37FD281E,0xB967539B
    ,0xA0FF312B,0x2E654AAE,0xD32BE808,0x5DB1938D,0x77501A5C,0xF9CA61D9,0x0484C37F,0x8A1EB8FA
    ,0xC742F21F,0x49D8899A,0xB4962B3C,0x3A0C50B9,0x10EDD968,0x9E77A2ED,0x6339004B,0xEDA37BCE
    ,0x25215026,0xABBB2BA3,0x56F58905,0xD86FF280,0xF28E7B51,0x7C1400D4,0x815AA272,0x0FC0D9F7
    ,0x429C9312,0xCC06E897,0x31484A31,0xBFD231B4,0x9533B865,0x1BA9C3E0,0xE6E76146,0x687D1AC3
    ,0xBFAE0D96,0x31347613,0xCC7AD4B5,0x42E0AF30,0x680126E1,0xE69B5D64,0x1BD5FFC2,0x954F8447
    ,0xD813CEA2,0x5689B527,0xABC71781,0x255D6C04,0x0FBCE5D5,0x81269E50,0x7C683CF6,0xF2F24773
    ,0x3A706C9B,0xB4EA171E,0x49A4B5B8,0xC73ECE3D,0xEDDF47EC,0x63453C69,0x9E0B9ECF,0x1091E54A
    ,0x5DCDAFAF,0xD357D42A,0x2E19768C,0xA0830D09,0x8A6284D8,0x04F8FF5D,0xF9B65DFB,0x772C267E},
    {0x00000000,0x7E4711FE,0x951344E0,0xEB54551E,0xFE24E2C9,0x8063F337,0x6B37A629,0x1570B7D7
    ,0xC751A68F,0xB916B771,0x5242E26F,0x2C05F391,0x39754446,0x473255B8,0xAC6600A6,0xD2211158
    ,0x314858B2,0x4F0F494C,0xA45B1C52,0xDA1C0DAC,0xCF6CBA7B,0xB12BAB85,0x5A7FFE9B,0x2438EF65
    ,0xF619FE3D,0x885EEFC3,0x630ABADD,0x1D4DAB23,0x083D1CF4,0x767A0D0A,0x9D2E5814,0xE36949EA
    ,0x8CEC5A91,0xF2AB4B6F,0x19FF1E71,0x67B80F8F,0x72C8B858,0x0C8FA9A6,0xE7DBFCB8,0x999CED46
    ,0x4BBDFC1E,0x35FAEDE0,0xDEAEB8FE,0xA0E9A900,0xB5991ED7,0xCBDE0F29,0x208A5A37,0x5ECD4BC9
    ,0xBDA40223,0xC3E313DD,0x28B746C3,0x56F0573D,0x4380E0EA,0x3DC7F114,0xD693A40A,0xA8D4B5F4
    ,0x7AF5A4AC,0x04B2B552,0xEFE6E04C,0x91A1F1B2,0x84D14665,0xFA96579B,0x11C20285,0x6F85137B
    ,0xD2A20667,0xACE51799,0x47B14287,0x39F65379,0x2C86E4AE,0x52C1F550,0xB995A04E,0xC7D2B1B0
    ,0x15F3A0E8,0x6BB4B116,0x80E0E408,0xFEA7F5F6,0xEBD74221,0x959053DF,0x7EC406C1,0x0083173F
    ,0xE3EA5ED5,0x9DAD4F2B,0x76F91A35,0x08BE0BCB,0x1DCEBC1C,0x6389ADE2,0x88DDF8FC,0xF69AE902
    ,0x24BBF85A,0x5AFCE9A4,0xB1A8BCBA,0xCFEFAD44,0xDA9F1A93,0xA4D80B6D,0x4F8C5E73,0x31CB4F8D
    ,0x5E4E5CF6,0x20094D08,0xCB5D1816,0xB51A09E8,0xA06ABE3F,0xDE2DAFC1,0x3579FADF,0x4B3EEB21
    ,0x991FFA79,0xE758EB87,0x0C0CBE99,0x724BAF67,0x673B18B0,0x197C094E,0xF2285C50,0x8C6F4DAE
    ,0x6F060444,0x114115BA,0xFA1540A4,0x8452515A,0x9122E68D,0xEF65F773,0x0431A26D,0x7A76B393
    ,0xA857A2CB,0xD610B335,0x3D44E62B,0x4303F7D5,0x56734002,0x283451FC,0xC36004E2,0xBD27151C
    ,0x616A84D7,0x1F2D9529,0xF479C037,0x8A3ED1C9,0x9F4E661E,0xE10977E0,0x0A5D22FE,0x741A3300
    ,0xA63B2258,0xD87C33A6,0x332866B8,0x4D6F7746,0x581FC091,0x2658D16F,0xCD0C8471,0xB34B958F
    ,0x5022DC65,0x2E65CD9B,0xC5319885,0xBB76897B,0xAE063EAC,0xD0412F52,0x3B157A4C,0x45526BB2
    ,0x97737AEA,0xE9346B14,0x02603E0A,0x7C272FF4,0x69579823,0x171089DD,0xFC44DCC3,0x8203CD3D
    ,0xED86DE46,0x93C1CFB8,0x78959AA6,0x06D28B58,0x13A23C8F,0x6DE52D71,0x86B1786F,0xF8F66991
    ,0x2AD778C9,0x54906937,0xBFC43C29,0xC1832DD7,0xD4F39A00,0xAAB48BFE,0x41E0DEE0,0x3FA7CF1E
    ,0xDCCE86F4,0xA289970A,0x49DDC214,0x379AD3EA,0x22EA643D,0x5CAD75C3,0xB7F920DD,0xC9BE3123
    ,0x1B9F207B,0x65D83185,0x8E8C649B,0xF0CB7565,0xE5BBC2B2,0x9BFCD34C,0x70A88652,0x0EEF97AC
    ,0xB3C882B0,0xCD8F934E,0x26DBC650,0x589CD7AE,0x4DEC6079,0x33AB7187,0xD8FF2499,0xA6B83567
    ,0x7499243F,0x0ADE35C1,0xE18A60DF,0x9FCD7121,0x8ABDC6F6,0xF4FAD708,0x1FAE8216,0x61E993E8
    ,0x8280DA02,0xFCC7CBFC,0x17939EE2,0x69D48F1C,0x7CA438CB,0x02E32935,0xE9B77C2B,0x97F06DD5
    ,0x45D17C8D,0x3B966D73,0xD0C2386D,0xAE852993,0xBBF59E44,0xC5B28FBA,0x2EE6DAA4,0x50A1CB5A
    ,0x3F24D821,0x4163C9DF,0xAA379CC1,0xD4708D3F,0xC1003AE8,0xBF472B16,0x54137E08,0x2A546FF6
    ,0xF8757EAE,0x86326F50,0x6D663A4E,0x13212BB0,0x06519C67,0x78168D99,0x9342D887,0xED05C979
    ,0x0E6C8093,0x702B916D,0x9B7FC473,0xE538D58D,0xF048625A,0x8E0F73A4,0x655B26BA,0x1B1C3744
    ,0xC93D261C,0xB77A37E2,0x5C2E62FC,0x22697302,0x3719C4D5,0x495ED52B,0xA20A8035,0xDC4D91CB},
    {0x00000000,0xB5481A12,0xC53C353D,0x70742F2F,0x7FBD4474,0xCAF55E66,0xBA817149,0x0FC96B5B
    ,0x51840CF5,0xE4CC16E7,0x94B839C8,0x21F023DA,0x2E394881,0x9B715293,0xEB057DBC,0x5E4D67AE
    ,0xB48DFE48,0x01C5E45A,0x71B1CB75,0xC4F9D167,0xCB30BA3C,0x7E78A02E,0x0E0C8F01,0xBB449513
    ,0xE509F2BD,0x5041E8AF,0x2035C780,0x957DDD92,0x9AB4B6C9,0x2FFCACDB,0x5F8883F4,0xEAC099E6
    ,0x2150CA73,0x9418D061,0xE46CFF4E,0x5124E55C,0x5EED8E07,0xEBA59415,0x9BD1BB3A,0x2E99A128
    ,0x70D4C686,0xC59CDC94,0xB5E8F3BB,0x00A0E9A9,0x0F6982F2,0xBA2198E0,0xCA55B7CF,0x7F1DADDD
    ,0x95DD343B,0x20952E29,0x50E10106,0xE5A91B14,0xEA60704F,0x5F286A5D,0x2F5C4572,0x9A145F60
    ,0xC45938CE,0x711122DC,0x01650DF3,0xB42D17E1,0xBBE47CBA,0x0EAC66A8,0x7ED84987,0xCB905395
    ,0xB2303494,0x07782E86,0x770C01A9,0xC2441BBB,0xCD8D70E0,0x78C56AF2,0x08B145DD,0xBDF95FCF
    ,0xE3B43861,0x56FC2273,0x26880D5C,0x93C0174E,0x9C097C15,0x29416607,0x59354928,0xEC7D533A
    ,0x06BDCADC,0xB3F5D0CE,0xC381FFE1,0x76C9E5F3,0x79008EA8,0xCC4894BA,0xBC3CBB95,0x0974A187
    ,0x5739C629,0xE271DC3B,0x9205F314,0x274DE906,0x2884825D,0x9DCC984F,0xEDB8B760,0x58F0AD72
    ,0x9360FEE7,0x2628E4F5,0x565CCBDA,0xE314D1C8,0xECDDBA93,0x5995A081,0x29E18FAE,0x9CA995BC
    ,0xC2E4F212,0x77ACE800,0x07D8C72F,0xB290DD3D,0xBD59B666,0x0811AC74,0x7865835B,0xCD2D9949
    ,0x27ED00AF,0x92A51ABD,0xE2D13592,0x57992F80,0x585044DB,0xED185EC9,0x9D6C71E6,0x28246BF4
    ,0x76690C5A,0xC3211648,0xB3553967,0x061D2375,0x09D4482E,0xBC9C523C,0xCCE87D13,0x79A06701
    ,0x0FBD7168,0xBAF56B7A,0xCA814455,0x7FC95E47,0x7000351C,0xC5482F0E,0xB53C0021,0x00741A33
    ,0x5E397D9D,0xEB71678F,0x9B0548A0,0x2E4D52B2,0x218439E9,0x94CC23FB,0xE4B80CD4,0x51F016C6
    ,0xBB308F20,0x0E789532,0x7E0CBA1D,0xCB44A00F,0xC48DCB54,0x71C5D146,0x01B1FE69,0xB4F9E47B
    ,0xEAB483D5,0x5FFC99C7,0x2F88B6E8,0x9AC0ACFA,0x9509C7A1,0x2041DDB3,0x5035F29C,0xE57DE88E
    ,0x2EEDBB1B,0x9BA5A109,0xEBD18E26,0x5E999434,0x5150FF6F,0xE418E57D,0x946CCA52,0x2124D040
    ,0x7F69B7EE,0xCA21ADFC,0xBA5582D3,0x0F1D98C1,0x00D4F39A,0xB59CE988,0xC5E8C6A7,0x70A0DCB5
    ,0x9A604553,0x2F285F41,0x5F5C706E,0xEA146A7C,0xE5DD0127,0x50951B35,0x20E1341A,0x95A92E08
    ,0xCBE449A6,0x7EAC53B4,0x0ED87C9B,0xBB906689,0xB4590DD2,0x011117C0,0x716538EF,0xC42D22FD
    ,0xBD8D45FC,0x08C55FEE,0x78B170C1,0xCDF96AD3,0xC2300188,0x77781B9A,0x070C34B5,0xB2442EA7
    ,0xEC094909,0x5941531B,0x29357C34,0x9C7D6626,0x93B40D7D,0x26FC176F,0x56883840,0xE3C02252
    ,0x0900BBB4,0xBC48A1A6,0xCC3C8E89,0x7974949B,0x76BDFFC0,0xC3F5E5D2,0xB381CAFD,0x06C9D0EF
    ,0x5884B741,0xEDCCAD53,0x9DB8827C,0x28F0986E,0x2739F335,0x9271E927,0xE205C608,0x574DDC1A
    ,0x9CDD8F8F,0x2995959D,0x59E1BAB2,0xECA9A0A0,0xE360CBFB,0x5628D1E9,0x265CFEC6,0x9314E4D4
    ,0xCD59837A,0x78119968,0x0865B647,0xBD2DAC55,0xB2E4C70E,0x07ACDD1C,0x77D8F233,0xC290E821
    ,0x285071C7,0x9D186BD5,0xED6C44FA,0x58245EE8,0x57ED35B3,0xE2A52FA1,0x92D1008E,0x27991A9C
    ,0x79D47D32,0xCC9C6720,0xBCE8480F,0x09A0521D,0x06693946,0xB3212354,0xC3550C7B,0x761D1669},
    {0x00000000,0x4526A804,0xE53AD776,0xA01C7F72,0xF3CFD67F,0xB6E97E7B,0x16F50109,0x53D3A90D
    ,0x9E2C126E,0xDB0ABA6A,0x7B16C518,0x3E306D1C,0x6DE3C411,0x28C56C15,0x88D91367,0xCDFFBB63
    ,0x123A1031,0x571CB835,0xF700C747,0xB2266F43,0xE1F5C64E,0xA4D36E4A,0x04CF1138,0x41E9B93C
    ,0x8C16025F,0xC930AA5B,0x692CD529,0x2C0A7D2D,0x7FD9D420,0x3AFF7C24,0x9AE30356,0xDFC5AB52
    ,0xEDD66BF4,0xA8F0C3F0,0x08ECBC82,0x4DCA1486,0x1E19BD8B,0x5B3F158F,0xFB236AFD,0xBE05C2F9
    ,0x73FA799A,0x36DCD19E,0x96C0AEEC,0xD3E606E8,0x8035AFE5,0xC51307E1,0x650F7893,0x2029D097
    ,0xFFEC7BC5,0xBACAD3C1,0x1AD6ACB3,0x5FF004B7,0x0C23ADBA,0x490505BE,0xE9197ACC,0xAC3FD2C8
    ,0x61C069AB,0x24E6C1AF,0x84FABEDD,0xC1DC16D9,0x920FBFD4,0xD72917D0,0x773568A2,0x3213C0A6
    ,0x36AB7D76,0x738DD572,0xD391AA00,0x96B70204,0xC564AB09,0x8042030D,0x205E7C7F,0x6578D47B
    ,0xA8876F18,0xEDA1C71C,0x4DBDB86E,0x089B106A,0x5B48B967,0x1E6E1163,0xBE726E11,0xFB54C615
    ,0x24916D47,0x61B7C543,0xC1ABBA31,0x848D1235,0xD75EBB38,0x9278133C,0x32646C4E,0x7742C44A
    ,0xBABD7F29,0xFF9BD72D,0x5F87A85F,0x1AA1005B,0x4972A956,0x0C540152,0xAC487E20,0xE96ED624
    ,0xDB7D1682,0x9E5BBE86,0x3E47C1F4,0x7B6169F0,0x28B2C0FD,0x6D9468F9,0xCD88178B,0x88AEBF8F
    ,0x455104EC,0x0077ACE8,0xA06BD39A,0xE54D7B9E,0xB69ED293,0xF3B87A97,0x53A405E5,0x1682ADE1
    ,0xC94706B3,0x8C61AEB7,0x2C7DD1C5,0x695B79C1,0x3A88D0CC,0x7FAE78C8,0xDFB207BA,0x9A94AFBE
    ,0x576B14DD,0x124DBCD9,0xB251C3AB,0xF7776BAF,0xA4A4C2A2,0xE1826AA6,0x419E15D4,0x04B8BDD0
    ,0x8AA2965E,0xCF843E5A,0x6F984128,0x2ABEE92C,0x796D4021,0x3C4BE825,0x9C579757,0xD9713F53
    ,0x148E8430,0x51A82C34,0xF1B45346,0xB492FB42,0xE741524F,0xA267FA4B,0x027B8539,0x475D2D3D
    ,0x9898866F,0xDDBE2E6B,0x7DA25119,0x3884F91D,0x6B575010,0x2E71F814,0x8E6D8766,0xCB4B2F62
    ,0x06B49401,0x43923C05,0xE38E4377,0xA6A8EB73,0xF57B427E,0xB05DEA7A,0x10419508,0x55673D0C
    ,0x6774FDAA,0x225255AE,0x824E2ADC,0xC76882D8,0x94BB2BD5,0xD19D83D1,0x7181FCA3,0x34A754A7
    ,0xF958EFC4,0xBC7E47C0,0x1C6238B2,0x594490B6,0x0A9739BB,0x4FB191BF,0xEFADEECD,0xAA8B46C9
    ,0x754EED9B,0x3068459F,0x90743AED,0xD55292E9,0x86813BE4,0xC3A793E0,0x63BBEC92,0x269D4496
    ,0xEB62FFF5,0xAE4457F1,0x0E582883,0x4B7E8087,0x18AD298A,0x5D8B818E,0xFD97FEFC,0xB8B156F8
    ,0xBC09EB28,0xF92F432C,0x59333C5E,0x1C15945A,0x4FC63D57,0x0AE09553,0xAAFCEA21,0xEFDA4225
    ,0x2225F946,0x67035142,0xC71F2E30,0x82398634,0xD1EA2F39,0x94CC873D,0x34D0F84F,0x71F6504B
    ,0xAE33FB19,0xEB15531D,0x4B092C6F,0x0E2F846B,0x5DFC2D66,0x18DA8562,0xB8C6FA10,0xFDE05214
    ,0x301FE977,0x75394173,0xD5253E01,0x90039605,0xC3D03F08,0x86F6970C,0x26EAE87E,0x63CC407A
    ,0x51DF80DC,0x14F928D8,0xB4E557AA,0xF1C3FFAE,0xA21056A3,0xE736FEA7,0x472A81D5,0x020C29D1
    ,0xCFF392B2,0x8AD53AB6,0x2AC945C4,0x6FEFEDC0,0x3C3C44CD,0x791AECC9,0xD90693BB,0x9C203BBF
    ,0x43E590ED,0x06C338E9,0xA6DF479B,0xE3F9EF9F,0xB02A4692,0xF50CEE96,0x551091E4,0x103639E0
    ,0xDDC98283,0x98EF2A87,0x38F355F5,0x7DD5FDF1,0x2E0654FC,0x6B20FCF8,0xCB3C838A,0x8E1A2B8E}};

#define CRC_BLOCK {\
			p1 = *cur32b++ ^ crc; p2 = *cur32b++;\
			crc = _crc32[0][(p2>>24) & 0xFF] ^ _crc32[1][(p2>>16) & 0xFF] ^ _crc32[2][(p2>> 8) & 0xFF] ^ _crc32[3][p2 & 0xFF] ^\
				  _crc32[4][(p1>>24) & 0xFF] ^ _crc32[5][(p1>>16) & 0xFF] ^ _crc32[6][(p1>> 8) & 0xFF] ^ _crc32[7][p1 & 0xFF]; }\
				 
uint32_t crc32 ( const void* data, size_t bytes, uint32_t initcrc )
{
	uint32_t crc = ~initcrc;
	const uint32_t *cur32b  = (const uint32_t*) data;

	uint32_t p1,p2;
	while (bytes >= 128)
	{
		for (uint32_t unroll = 0; unroll < 16; unroll++) CRC_BLOCK
		bytes -= 128;
	}

	while (bytes >= 8)
	{
		CRC_BLOCK
		bytes -= 8;
	}
	
	const uint8_t* cur8b = (const uint8_t*) cur32b;
	const uint8_t* end_cur8b = cur8b + bytes;
	while (cur8b < end_cur8b) {		
		crc = (crc >> 8) ^ _crc32[0][(crc & 0xFF) ^ *cur8b++];
	}

	return ~crc;
}

#undef CRC_BLOCK
