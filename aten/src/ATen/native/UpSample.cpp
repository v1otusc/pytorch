// Copyright 2004-present Facebook. All Rights Reserved.
#define TORCH_ASSERT_ONLY_METHOD_OPERATORS

#include <ATen/native/UpSample.h>
#include <c10/util/irange.h>
#include <c10/util/TypeCast.h>

namespace at {
namespace native {
namespace upsample {

TORCH_API c10::SmallVector<int64_t, 3> compute_output_size(
    c10::IntArrayRef input_size,  // Full input tensor size.
    at::OptionalIntArrayRef output_size,
    c10::optional<c10::ArrayRef<double>> scale_factors,
    bool round_with_scale_factor) {
  const auto spatial_dimensions = static_cast<int64_t>(input_size.size()) - 2;
  if (output_size) {
    TORCH_CHECK(!scale_factors, "Must specify exactly one of output_size and scale_factors");
    TORCH_CHECK(static_cast<int64_t>(output_size->size()) == spatial_dimensions);
    return {output_size->data(), output_size->data() + output_size->size()};
  }
  if (scale_factors) {
    TORCH_CHECK(!output_size, "Must specify exactly one of output_size and scale_factors");
    TORCH_CHECK(static_cast<int64_t>(scale_factors->size()) == spatial_dimensions);
    c10::SmallVector<int64_t, 3> ret;
    for (const auto i : c10::irange(spatial_dimensions)) {
      const double d = round_with_scale_factor ? 0.5 : 0.0;
      // if round_with_scale_factor=true we perform round (i.e. int(0.5 + x)) to match opencv, scipy,
      // scikit-image output size
      const double odim = d + static_cast<double>(input_size[i+2]) * scale_factors.value()[i];
      ret.push_back(c10::checked_convert<int64_t>(odim, "int64_t"));
    }
    return ret;
  }
  TORCH_CHECK(false, "Must specify exactly one of output_size and scale_factors");
}

} // namespace upsample
} // namespace native
} // namespace at
