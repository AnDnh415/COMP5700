#include <llvm/ADT/SmallVector.h>
#include <iostream>

void appendIntelLLVMSpirvOptions(llvm::SmallVector<std::string>& out) {
  llvm::SmallVector<std::string> Args {"-spirv-max-version=1.3",
      "-spirv-debug-info-version=ocl-100",
      "-spirv-allow-extra-diexpressions",
      "-spirv-allow-unknown-intrinsics=llvm.genx.",
      "-spirv-ext=-all,+SPV_EXT_shader_atomic_float_add,+SPV_EXT_shader_atomic_float_min_max,+SPV_"
      "KHR_no_integer_wrap_decoration,+SPV_KHR_float_controls,+SPV_KHR_expect_assume,+SPV_INTEL_"
      "subgroups,+SPV_INTEL_media_block_io,+SPV_INTEL_device_side_avc_motion_estimation,+SPV_INTEL_"
      "fpga_loop_controls,+SPV_INTEL_fpga_memory_attributes,+SPV_INTEL_fpga_memory_accesses,+SPV_"
      "INTEL_unstructured_loop_controls,+SPV_INTEL_fpga_reg,+SPV_INTEL_blocking_pipes,+SPV_INTEL_"
      "function_pointers,+SPV_INTEL_kernel_attributes,+SPV_INTEL_io_pipes,+SPV_INTEL_inline_"
      "assembly,+SPV_INTEL_arbitrary_precision_integers,+SPV_INTEL_float_controls2,+SPV_INTEL_"
      "vector_compute,+SPV_INTEL_fast_composite,+SPV_INTEL_fpga_buffer_location,+SPV_INTEL_joint_"
      "matrix,+SPV_INTEL_arbitrary_precision_fixed_point,+SPV_INTEL_arbitrary_precision_floating_"
      "point,+SPV_INTEL_arbitrary_precision_floating_point,+SPV_INTEL_variable_length_array,+SPV_"
      "INTEL_fp_fast_math_mode,+SPV_INTEL_fpga_cluster_attributes,+SPV_INTEL_loop_fuse,+SPV_INTEL_"
      "long_constant_composite,+SPV_INTEL_fpga_invocation_pipelining_attributes,+SPV_INTEL_fpga_"
      "dsp_control,+SPV_INTEL_arithmetic_fence,+SPV_INTEL_runtime_aligned,"
      "+SPV_INTEL_optnone,+SPV_INTEL_token_type,+SPV_INTEL_bfloat16_conversion,+SPV_INTEL_joint_"
      "matrix,+SPV_INTEL_hw_thread_queries,+SPV_INTEL_memory_access_aliasing,+SPV_EXT_relaxed_printf_string_address_space"
  };
  for(const auto& S : Args) {
    out.push_back(S);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, std::size_t Size)
{
  if (Size > 4) 
  {
    llvm::SmallVector<std::string> myVec;
    /*
    * iterate the size of incoming data with increments of 4 adress spaces and
    * Cast the Data ptr from uint8_t to char, convert it to a string, and add it  
    */
    std::size_t sliceSize = 4;
    for (std::size_t i = 0; i < Size; i += sliceSize) 
    {
      std::size_t length = std::min(sliceSize, Size - i);
      std::string str(reinterpret_cast<const char*>(Data + i), length);
      myVec.push_back(str);
    }
    appendIntelLLVMSpirvOptions(myVec);
  }
  return 0;
} 