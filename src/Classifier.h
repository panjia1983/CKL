#ifndef CKL_CLASSIFIER_H
#define CKL_CLASSIFIER_H

#include "svm.h"
#include <vector>
#include <limits>

namespace CKL
{

template<std::size_t N>
class Vecnf
{
public:
  Vecnf()
  {
    data.resize(N, 0);
  }
  
  template<std::size_t M>
  Vecnf(const Vecnf<M>& data_)
  {
    std::size_t n = std::min(M, N);
    for(std::size_t i = 0; i < n; ++i)
      data[i] = data_[i];
  }
  
  Vecnf(const std::vector<CKL_REAL>& data_)
  {
    data.resize(N, 0);
    std::size_t n = std::min(data_.size(), N);
    for(std::size_t i = 0; i < n; ++i)
      data[i] = data_[i];
  }
  
  bool operator == (const Vecnf<N>& other) const
  {
    for(std::size_t i = 0; i < N; ++i)
    {
      if(data[i] != other[i]) return false;
    }
    return true;
  }
  
  bool operator != (const Vecnf<N>& other) const
  {
    for(std::size_t i = 0; i < N; ++i)
    {
      if(data[i] != other[i]) return true;
    }
    
    return false;
  }
  
  
  std::size_t dim() const
  {
    return N;
  }
  
  void setData(const std::vector<CKL_REAL>& data_)
  {
    std::size_t n = std::min(data.size(), N);
    for(std::size_t i = 0; i < n; ++i)
      data[i] = data_[i];
  }
  
  CKL_REAL operator [](std::size_t i) const
  {
    return data[i];
  }
  
  CKL_REAL& operator [](std::size_t i)
  {
    return data[i];
  }
  
  Vecnf<N> operator + (const Vecnf<N>& other) const
  {
    Vecnf<N> result;
    for(std::size_t i = 0; i < N; ++i)
      result[i] = data[i] + other[i];
    return result;
  }
  
  Vecnf<N>& operator += (const Vecnf<N>& other)
  {
    for(std::size_t i = 0; i < N; ++i)
      data[i] += other[i];
    return *this;
  }
  
  Vecnf<N> operator - (const Vecnf<N>& other) const
  {
    Vecnf<N> result;
    for(std::size_t i = 0; i < N; ++i)
      result[i] = data[i] - other[i];
    return result;
  }
  
  Vecnf<N>& operator -= (const Vecnf<N>& other)
  {
    for(std::size_t i = 0; i < N; ++i)
      data[i] -= other[i];
    return *this;
  }
  
  Vecnf<N> operator * (CKL_REAL t) const
  {
    Vecnf<N> result;
    for(std::size_t i = 0; i < N; ++i)
      result[i] = data[i] * t;
    return result;
  }
  
  Vecnf<N>& operator *= (CKL_REAL t)
  {
    for(std::size_t i = 0; i < N; ++i)
      data[i] *= t;
    return *this;
  }
  
  Vecnf<N> operator / (CKL_REAL t) const
  {
    Vecnf<N> result;
    for(std::size_t i = 0; i < N; ++i)
      result[i] = data[i] / 5;
    return result;
  }
  
  Vecnf<N>& operator /= (CKL_REAL t)
  {
    for(std::size_t i = 0; i < N; ++i)
      data[i] /= t;
    return *this;
  }
  
  Vecnf<N>& setZero()
  {
    for(std::size_t i = 0; i < N; ++i)
      data[i] = 0;
  }
  
  CKL_REAL dot(const Vecnf<N>& other) const
  {
    CKL_REAL sum = 0;
    for(std::size_t i = 0; i < N; ++i)
      sum += data[i] * other[i];
    return sum;
  }
  
  std::vector<CKL_REAL> getData() const
  {
    return data;
  }
  
protected:
  std::vector<CKL_REAL> data;
};

template<std::size_t N>
Vecnf<N> operator * (CKL_REAL t, const Vecnf<N>& v)
{
  return v * t;
}


template<std::size_t N>
std::ostream& operator << (std::ostream& o, const Vecnf<N>& v)
{
  o << "(";
  for(std::size_t i = 0; i < N; ++i)
  {
    if(i == N - 1)
      o << v[i] << ")";
    else
      o << v[i] << " ";
  }
  return o;
}








template<std::size_t N>
struct Item
{
  Vecnf<N> q;
  bool label;
  CKL_REAL w;
  
  Item(const Vecnf<N>& q_, bool label_, CKL_REAL w_ = 1) : q(q_),
    label(label_),
    w(w_)
  {}
  
  Item() {}
};

template<std::size_t N>
struct Scaler
{
  Vecnf<N> v_min, v_max;
  Scaler()
  {
    // default no scale
    for(std::size_t i = 0; i < N; ++i)
    {
      v_min[i] = 0;
      v_max[i] = 1;
    }
  }
  
  Scaler(const Vecnf<N>& v_min_, const Vecnf<N>& v_max_) : v_min(v_min_),
    v_max(v_max_)
  {}
  
  Vecnf<N> scale(const Vecnf<N>& v) const
  {
    Vecnf<N> res;
    for(std::size_t i = 0; i < N; ++i)
      res[i] = (v[i] - v_min[i]) / (v_max[i] - v_min[i]);
    return res;
  }
  
  Vecnf<N> unscale(const Vecnf<N>& v) const
  {
    Vecnf<N> res;
    for(std::size_t i = 0; i < N; ++i)
      res[i] = v[i] * (v_max[i] - v_min[i]) + v_min[i];
    return res;
  }
};


struct PredictResult
{
  bool label;
  CKL_REAL prob;
  
  PredictResult() {}
  PredictResult(bool label_, CKL_REAL prob_ = 1) : label(label_),
    prob(prob_)
  {}
};


template<std::size_t N>
class Classifier
{
public:
  Classifier()
  {
    param.svm_type = C_SVC;
    param.kernel_type = RBF;
    param.degree = 3;
    param.gamma = 0;	// 1/num_features
    param.coef0 = 0;
    param.nu = 0.5;
    param.cache_size = 100; // can change
    param.C = 1;
    param.eps = 1e-3;
    param.p = 0.1;
    param.shrinking = 1;    // use shrinking
    param.probability = 0;
    param.nr_weight = 0;
    param.weight_label = NULL;
    param.weight = NULL;
    
    param.nr_weight = 2;
    param.weight_label = (int *)realloc(param.weight_label, sizeof(int) * param.nr_weight);
    param.weight = (double *)realloc(param.weight, sizeof(double) * param.nr_weight);
    param.weight_label[0] = -1;
    param.weight_label[1] = 1;
    param.weight[0] = 1;
    param.weight[1] = 1;
    
    model = NULL;
    x_space = NULL;
    problem.x = NULL;
    problem.y = NULL;
    problem.W = NULL;
  }
  
  void setScaler(const Scaler<N>& scaler_)
  {
    scaler = scaler_;
  }
  
  void setEPS(CKL_REAL e)
  {
    param.eps = e;
  }
  
  void setGamma(CKL_REAL gamma)
  {
    param.gamma = gamma;
  }
  
  CKL_REAL error_rate(const std::vector<Item<N> >& data) const
  {
    std::size_t num = data.size();
    
    std::size_t error_num = 0;
    for(std::size_t i = 0; i < data.size(); ++i)
    {
      PredictResult res = predict(data[i].q);
      if(res.label != data[i].label)
        error_num++;
    }
    
    return error_num / (CKL_REAL)num;
  }
  
  ~Classifier()
  {
    svm_destroy_param(&param);
    svm_free_and_destroy_model(&model);
    delete [] x_space;
    delete [] problem.x;
    delete [] problem.y;
    delete [] problem.W;
  }
  
  void learn(const std::vector<Item<N> >& data)
  {
    if(data.size() == 0) return;
    
    if(model) svm_free_and_destroy_model(&model);
    if(param.gamma == 0) param.gamma = 1.0 / N;
    
    problem.l = data.size();
    if(problem.y) delete [] problem.y;
    problem.y = new double [problem.l];
    if(problem.x) delete [] problem.x;
    problem.x = new svm_node* [problem.l];
    if(problem.W) delete [] problem.W;
    problem.W = new double [problem.l];
    if(x_space) delete [] x_space;
    x_space = new svm_node [(N + 1) * problem.l];
    
    for(std::size_t i = 0; i < data.size(); ++i)
    {
      svm_node* cur_x_space = x_space + (N + 1) * i;
      Vecnf<N> q_scaled = scaler.scale(data[i].q);
      for(std::size_t j = 0; j < N; ++j)
      {
        cur_x_space[j].index = j + 1;
        cur_x_space[j].value = q_scaled[j];
      }
      cur_x_space[N].index = -1;
      
      problem.x[i] = cur_x_space;
      problem.y[i] = (data[i].label ? 1 : -1);
      problem.W[i] = data[i].w;
    }
    
    model = svm_train(&problem, &param);
    hyperw_normsqr = svm_hyper_w_normsqr_twoclass(model);
  }
  
  std::vector<PredictResult> predict(const std::vector<Vecnf<N> >& qs) const
  {
    std::vector<PredictResult> predict_results;
    
    int nr_class = svm_get_nr_class(model);
    double* prob_estimates = NULL;
    
    svm_node* x = (svm_node*)malloc((N + 1) * sizeof(svm_node));
    if(param.probability)
      prob_estimates = (double*)malloc(nr_class * sizeof(double));
      
    Vecnf<N> v;
    for(std::size_t i = 0; i < qs.size(); ++i)
    {
      v = scaler.scale(qs[i]);
      for(std::size_t j = 0; j < N; ++j)
      {
        x[j].index = j + 1;
        x[j].value = v[j];
      }
      x[N].index = -1;
      
      double predict_label;
      
      if(param.probability)
      {
        predict_label = svm_predict_probability(model, x, prob_estimates);
        predict_label = (predict_label > 0) ? 1 : 0;
        predict_results.push_back(PredictResult(predict_label, *prob_estimates));
      }
      else
      {
        predict_label = svm_predict(model, x);
        predict_label = (predict_label > 0) ? 1 : 0;
        predict_results.push_back(PredictResult(predict_label));
      }
    }
    
    if(param.probability) free(prob_estimates);
    free(x);
    
    return predict_results;
  }
  
  PredictResult predict(const Vecnf<N>& q) const
  {
    return (predict(std::vector<Vecnf<N> >(1, q)))[0];
  }
  
  void save(const std::string& filename) const
  {
    if(model)
      svm_save_model(filename.c_str(), model);
  }
  
  std::vector<Item<N> > getSupportVectors() const
  {
    std::vector<Item<N> > results;
    Item<N> item;
    for(std::size_t i = 0; i < (std::size_t)model->l; ++i)
    {
      for(std::size_t j = 0; j < N; ++j)
        item.q[j] = model->SV[i][j].value;
      item.q = scaler.unscale(item.q);
      int id = model->sv_indices[i] - 1;
      item.label = (problem.y[id] > 0);
      results.push_back(item);
    }
    
    return results;
  }
  
  svm_parameter param;
  svm_problem problem;
  svm_node* x_space;
  svm_model* model;
  double hyperw_normsqr;
  
  Scaler<N> scaler;
};


template<std::size_t N>
Scaler<N> computeScaler(const std::vector<Item<N> >& data)
{
  Vecnf<N> lower_bound, upper_bound;
  for(std::size_t j = 0; j < N; ++j)
  {
    lower_bound[j] = std::numeric_limits<CKL_REAL>::max();
    upper_bound[j] = -std::numeric_limits<CKL_REAL>::max();
  }
  
  for(std::size_t i = 0; i < data.size(); ++i)
  {
    for(std::size_t j = 0; j < N; ++j)
    {
      if(data[i].q[j] < lower_bound[j]) lower_bound[j] = data[i].q[j];
      if(data[i].q[j] > upper_bound[j]) upper_bound[j] = data[i].q[j];
    }
  }
  
  return Scaler<N>(lower_bound, upper_bound);
}

template<std::size_t N>
Scaler<N> computeScaler(const std::vector<Vecnf<N> >& data)
{
  Vecnf<N> lower_bound, upper_bound;
  for(std::size_t j = 0; j < N; ++j)
  {
    lower_bound[j] = std::numeric_limits<CKL_REAL>::max();
    upper_bound[j] = -std::numeric_limits<CKL_REAL>::max();
  }
  
  for(std::size_t i = 0; i < data.size(); ++i)
  {
    for(std::size_t j = 0; j < N; ++j)
    {
      if(data[i][j] < lower_bound[j]) lower_bound[j] = data[i][j];
      if(data[i][j] > upper_bound[j]) upper_bound[j] = data[i][j];
    }
  }
  
  return Scaler<N>(lower_bound, upper_bound);
}



}

#endif
