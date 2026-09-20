/* ===========================================================================
   Exercise 01 — Performance metrics and the roofline, in code
   Roll Number: CB.AI.U4AIM24102
   =========================================================================== */

#include "metrics.h"
#include <math.h>

/* Speedup S = t1 / tN. Returns 0.0 if tN <= 0. */
double speedup(double t1, double tN)
{
    if (tN <= 0.0 || t1 <= 0.0) return 0.0;
    return t1 / tN;
}

/* Efficiency E = S / N. Returns 0.0 if N <= 0 or tN <= 0. */
double efficiency(double t1, double tN, int N)
{
    if (N <= 0 || tN <= 0.0 || t1 <= 0.0) return 0.0;
    return (t1 / tN) / (double)N;
}

/* Amdahl's law: maximum speedup on N processors with serial fraction s.
   S = 1 / (s + (1-s)/N).  Returns 0.0 if N <= 0 or s outside [0,1]. */
double amdahl(double s, int N)
{
    if (N <= 0 || s < 0.0 || s > 1.0) return 0.0;
    double denom = s + (1.0 - s) / (double)N;
    if (denom <= 0.0) return 0.0;
    return 1.0 / denom;
}

/* The asymptotic Amdahl ceiling, 1/s. Returns -1.0 for s <= 0 (unbounded). */
double amdahl_limit(double s)
{
    if (s <= 0.0) return -1.0;
    if (s > 1.0) return 0.0;
    return 1.0 / s;
}

/* Gustafson's law: S = N - s*(N-1). Returns 0.0 if N <= 0 or s outside [0,1]. */
double gustafson(double s, int N)
{
    if (N <= 0 || s < 0.0 || s > 1.0) return 0.0;
    return (double)N - s * (double)(N - 1);
}

/* Karp-Flatt experimentally determined serial fraction:
   e = (1/S - 1/N) / (1 - 1/N).  Returns -1.0 if N < 2 or S <= 0. */
double karp_flatt(double S, int N)
{
    if (N < 2 || S <= 0.0) return -1.0;
    double term1 = (1.0 / S) - (1.0 / (double)N);
    double term2 = 1.0 - (1.0 / (double)N);
    return term1 / term2;
}

/* Arithmetic intensity I = flops / bytes, in FLOP per byte.
   Returns 0.0 if bytes <= 0. */
double arithmetic_intensity(double flops, double bytes)
{
    if (bytes <= 0.0 || flops < 0.0) return 0.0;
    return flops / bytes;
}

/* Ridge point of a machine, in FLOP per byte: peak_gflops / bandwidth_gbs.
   Returns 0.0 if bandwidth_gbs <= 0. */
double ridge_point(double peak_gflops, double bandwidth_gbs)
{
    if (bandwidth_gbs <= 0.0 || peak_gflops <= 0.0) return 0.0;
    return peak_gflops / bandwidth_gbs;
}

/* Attainable performance in GFLOP/s: min(peak, bandwidth * I). */
double attainable_gflops(double peak_gflops, double bandwidth_gbs, double intensity)
{
    if (peak_gflops <= 0.0 || bandwidth_gbs <= 0.0 || intensity <= 0.0) return 0.0;
    double bw_cap = bandwidth_gbs * intensity;
    return (peak_gflops < bw_cap) ? peak_gflops : bw_cap;
}

/* 1 if the kernel is memory bound on this machine (I < ridge), else 0.
   A kernel exactly at the ridge counts as compute bound. */
int is_memory_bound(double peak_gflops, double bandwidth_gbs, double intensity)
{
    if (peak_gflops <= 0.0 || bandwidth_gbs <= 0.0 || intensity <= 0.0) return 0;
    double r = ridge_point(peak_gflops, bandwidth_gbs);
    return (intensity < r) ? 1 : 0;
}

/* Fraction of the ATTAINABLE ceiling actually achieved, in [0,1].
   Returns 0.0 if the attainable rate is not positive. */
double roofline_fraction(double peak_gflops, double bandwidth_gbs,
                         double intensity, double achieved_gflops)
{
    double attainable = attainable_gflops(peak_gflops, bandwidth_gbs, intensity);
    if (attainable <= 0.0 || achieved_gflops <= 0.0) return 0.0;
    double frac = achieved_gflops / attainable;
    if (frac > 1.0) return 1.0;
    return frac;
}
