#ifndef IDEEP_OPERATORS_LAYERNORM_HPP
#define IDEEP_OPERATORS_LAYERNORM_HPP

namespace ideep {

struct layer_normalization_forward : public dnnl::layer_normalization_forward {
  using super = dnnl::layer_normalization_forward;
  static void compute(const tensor& src,
                      const tensor& scale,
                      const tensor& shift,
                      tensor& dst,
                      tensor& mean,
                      tensor& variance,
                      float epsilon,
                      const engine& aengine = engine::cpu_engine()) {
    auto flags = batch_normalization_flag::use_scale_shift;
    auto src_desc = src.get_desc();
    auto pd = primitive_desc(
        {prop_kind::forward_training, src_desc, epsilon, flags}, aengine);
    tensor scale_shift {pd.weights_desc()};
    std::memcpy(scale_shift.get_data_handle(),
              scale.get_data_handle(), scale.get_size());
    std::memcpy(scale_shift.get_data_handle() + scale.get_size(),
                shift.get_data_handle(), shift.get_size());
    auto expected_src = src.reorder_if_differ_in(pd.src_desc());
    mean.reinit_if_necessary(pd.mean_desc());
    variance.reinit_if_necessary(pd.variance_desc());
    dst.reinit_if_necessary(pd.dst_desc());
    super(pd).execute(stream::default_stream(),
            {{DNNL_ARG_SRC, expected_src},
            {DNNL_ARG_SCALE_SHIFT, scale_shift},
            {DNNL_ARG_MEAN, mean},
            {DNNL_ARG_VARIANCE, variance},
            {DNNL_ARG_DST, dst}});
  }
};

struct layer_normalization_backward: public dnnl::layer_normalization_backward {
  static inline void compute() {
  }
};

}  // namespace ideep

#endif
