// Ensures the selected variant's hardware glue is compiled into the build.
// We gate this include on a variant-specific macro to avoid cross-variant collisions.

#if defined(VARIANT_T5S3_EINK_PRO)
#include "../variants/t5s3_eink_pro/target.cpp"
#endif
