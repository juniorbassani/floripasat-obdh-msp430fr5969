################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
floripasat_not_working/driver/%.obj: ../floripasat_not_working/driver/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/home/junior/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/bin/cl430" -vmspx --abi=eabi --data_model=large -O0 --use_hw_mpy=F5 --include_path="/home/junior/ti/ccs910/ccs/ccs_base/msp430/include" --include_path="/home/junior/junior/ccs/floripasat" --include_path="/tmp/FreeRTOSv10.2.1/FreeRTOS/Source/include" --include_path="/tmp/FreeRTOSv10.2.1/FreeRTOS/Source/portable/CCS/MSP430X" --include_path="/home/junior/ti/ccs910/ccs/tools/compiler/ti-cgt-msp430_18.12.2.LTS/include" --include_path="/home/junior/junior/ccs/floripasat/driverlib/MSP430FR5xx_6xx" -g --define=__MSP430FR5969__ --display_error_number --diag_warning=225 --diag_wrap=off --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="floripasat_not_working/driver/$(basename $(<F)).d_raw" --obj_directory="floripasat_not_working/driver" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

