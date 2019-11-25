# Instructions

## Install Code Composer Studio

Download Code Composer Studio (http://software-dl.ti.com/ccs/esd/documents/ccs_downloads.html) and enable support for MSP430 microcontrollers when installing.

Install the necessary drivers for the microcontrollers. In ubuntu, by default, it's in $HOME/ti/ccs910/ccs/install_scripts/install_drivers.sh

## Install This Project

### Download

```
git clone https://github.com/juniorbassani/floripasat-obdh-msp430fr5969.git
```

1. Open Code Composer Studio.
2. Go to File -> Import… -> Code Composer Studio -> CCS Projects -> Browse.
3. Select the directory in which the repository was downloaded and click Finish.

### Compile and Run

1. Right-click Project Explorer -> Build Project.
2. Connect the microcontroller to the host computer.
3. Click Run -> Debug. 

If the Launching Debug Session window pops up, select the appropriate microcontroller and click OK. Wait for the code to finish uploading and click the Play button or press F8. To terminate the debug session, click Terminate.

**Important**: If the project does not compile and results in the error message: "conversion u not supported when using printf_support=minimal mode", do as follows:

1. Click Project -> Show Building Settings -> Build -> Compiler -> Advanced Options -> Library Function Assumptions.
2. Change Level of printf/scanf support to full
