################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
uC-CPU/MSP430/CCS/cpu_a.obj: ../uC-CPU/MSP430/CCS/cpu_a.s43 $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"G:/DeveloperFiles/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.3/bin/cl430" -vmspx --abi=eabi --data_model=restricted --use_hw_mpy=F5 --include_path="G:/DeveloperFiles/ti/ccsv6/ccs_base/msp430/include" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/BSP/cJSON" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/BSP/Platform" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/BSP/utils" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/FatFs" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/BSP/Devicelib" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/BSP/Hallib" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/App" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/uC-CPU" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/uC-CPU/MSP430/CCS" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/uC-LIB" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/uCOS-II/Ports/MSP430X/CCS" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/uCOS-II/Source" --include_path="G:/04  MyDesign/04  CCS/glz_msp430_ucOS_II/BSP" --include_path="G:/DeveloperFiles/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.3/include" --advice:power=all -g --define=__MSP430F5259__ --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="uC-CPU/MSP430/CCS/cpu_a.pp" --obj_directory="uC-CPU/MSP430/CCS" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


