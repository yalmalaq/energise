# Rails documentation

## Rails on both devices


| Device 0                                  | Device 1                       |
|-------------------------------------------|--------------------------------|
| BUCK1M(S1M_VDD_MIF):MIF                   | BUCK1S(S1S_VDD_CAM):Multimedia |
| **BUCK2M(S2M_VDD_CPUCL2):CPU(BIG)**       | BUCK2S(S2S_VDD_G3D):GPU        |
| **BUCK3M(S3M_VDD_CPUCL1):CPU(MID)**       | BUCK3S(S3S_LLDO1):LDO          |
| **BUCK4M(S4M_VDD_CPUCL0):CPU(LITTLE)**    | BUCK4S(S4S_VDD2H_MEM):DDR      |
| BUCK5M(S5M_VDD_INT):INT                   | BUCK5S(S5S_VDDQ_MEM):DDR       |
| **BUCK6M(S6M_VDD_CPUCL2_M):CPU(BIG)**     | BUCK6S(S6S_LLDO4):LDO          |
| BUCK7M(S7M_VDD_INT_M):INT                 | BUCK7S(S7S_MLDO):LDO           |
| BUCK8M(S8M_LLDO2):LDO                     | BUCK8S(S8S_VDD_G3D_L2):GPU     |
| BUCK9M(S9M_LLDO3):LDO                     | BUCK9S(S9S_VDD_AOC):AOC        |
| BUCK10M(S10M_VDD_TPU):TPU                 | BUCK10S(S10S_VDD2L):DDR        |
| LDO1M(L1M_ALIVE):Alive                    | BUCKD(SD):UFS                  |
| LDO2M(L2M_ALIVE):DDR                      | BUCKA(SA):IO                   |
| LDO3M(L3M):IO                             | BUCKBOOST(BB_HLDO):LDO         |
| LDO4M(L4M):IO                             | LDO1S(L1S_VDD_G3D_M):GPU       |
| LDO5M(L5M_TCXO):PLL                       | LDO2S(L2S_VDD_AOC_RET):AOC     |
| LDO6M(L6M_PLL):PLL                        | LDO3S(L3S_UNUSED):Spare        |
| LDO7M(L7M_VDD_HSI):IO                     | LDO4S(L4S_UWB):UWB             |
| LDO8M(L8M_USB):USB                        | LDO5S(L5S_UNUSED):Spare        |
| LDO9M(L9M_USB):USB                        | LDO6S(L6S_PROX):Sensors        |
| LDO10M(L10M_USB):USB                      | LDO7S(L7S_SENSORS):Sensors     |
| **LDO11M(L11M_VDD_CPUCL1_M):CPU(MID)**    | LDO8S(L8S_UFS_VCCQ):UFS        |
| **LDO12M(L12M_VDD_CPUCL0_M):CPU(LITTLE)** | LDO9S(L9S_GNSS_CORE):GPS       |
| LDO13M(L13M_VDD_TPU_M):TPU                | LDO10S(L10S_GNSS_RF):GPS       |
| LDO14M(L14M_AVDD_TCXO):PLL                | LDO11S(L11S_GNSS_AUX):GPS      |
| LDO15M(L15M_VDD_SLC_M):SLC                | LDO12S(L12S_UNUSED):Spare      |
| LDO16M(L16M_PCIE0):PCIE                   | LDO13S(L13S_DPAUX):Display     |
| LDO17M(L17M_PCIE1):PCIE                   | LDO14S(L14S_UWB):UWB           |
| LDO18M(L18M_PCIE0):PCIE                   | LDO15S(L15S_UNUSED):Spare      |
| LDO19M(L19M_PCIE1):PCIE                   | VSEN_C4(VSEN_C4_NC):Spare      |
| LDO20M(L20M_DMICS):DMIC                   | VSEN_C5(VSEN_C5_NC):Spare      |
| LDO21M(L21M_DTLS):DTLS                    | VSEN_C6(VSEN_C6_NC):Spare      |
| LDO22M(L22M_DISP_VCI):Display             |                                |
| LDO23M(L23M_DISP_VDDI):Display            |                                |
| LDO24M(L24M_UNUSED):Spare                 |                                |
| LDO25M(L25M_TSP_DVDD):Touch               |                                |
| LDO26M(L26M_TS_AVDD):Touch                |                                |
| LDO27M(L27M_UDFPS_AVDD):UDFPS             |                                |
| LDO28M(L28M_DISP_VDDD):Display            |                                |
| LDO29M(L29M_UNUSED):Spare                 |                                |
| LDO30M(L30M_UNUSED):Spare                 |                                |
| LDO31M(L31M_DISP_VDDI):NFC                |                                |
| **VSEN_C1(VSYS_PWR_DISPLAY):Display**     |                                |
| VSEN_C2(VSEN_C2_NC):Spare                 |                                |
| VSEN_C3(VSEN_C3_NC):Spare                 |                                |


## Paths

/sys/bus/iio/devices/iio:device0\
/sys/bus/iio/devices/iio:device1




