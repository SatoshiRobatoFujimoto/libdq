

#include "dq.h"
#include "mat3.h"

#include <stdio.h>
#include <math.h>

#include <sys/time.h>


static int test_vector (void)
{
   double v0[3] = { 3., -3., 1. };
   double v1[3] = { 4., 9., 2. };
   double v2[3] = { -12., 12., -4. };
   double u[3];
   double u01[3] = { -15., -2., 39. };
   double u02[3] = { 0., 0., 0. };

   /* First test. */
   vec3_cross( u, v0, v1 );
   if (vec3_cmp( u, u01 )) {
      fprintf( stderr, "Error with cross product!\n" );
      printf("Got: %0.3f, %0.3f, %0.3f", u[0], u[1], u[2]);
      printf("Expected: %0.3f, %0.3f, %0.3f", u01[0], u01[1], u01[2]);
      return -1;
   }

   /* Second test. */
   vec3_cross( u, v0, v2 );
   if (vec3_cmp( u, u02 )) {
      fprintf( stderr, "Error with cross product!\n" );
      printf("Got: %0.3f, %0.3f, %0.3f", u[0], u[1], u[2]);
      printf("Expected: %0.3f, %0.3f, %0.3f", u02[0], u02[1], u02[2]);
      return -1;
   }

   return 0;
}


static int test_matrix (void)
{
   double M[3][3] = { { 1., 3., 1. },
                      { 1., 1., 2. },
                      { 2., 3., 4. } };
   double comp[3][3] = { { 2., 9., -5. },
                         { 0., -2., 1. },
                         { -1., -3., 2. } };
   double eye[3][3] = { { 1., 0., 0. },
                        { 0., 1., 0. },
                        { 0., 0., 1. } };
   double det, inv[3][3], mmul[3][3];

   det = mat3_det( M );
   if (fabs(det+1.) > 1e-10) {
      fprintf( stderr, "Error with determinant, got %.3e expected -1.\n", det );
      return -1;
   }

   mat3_inv( inv, M );
   if (mat3_cmp( inv, comp ) != 0) {
      fprintf( stderr, "Error with matrix inversion!\n" );
      printf( "Got:\n" );
      mat3_print( inv );
      printf( "Expected:\n" );
      mat3_print( comp );
      return -1;
   }

   mat3_mul( mmul, M, inv );
   if (mat3_cmp( mmul, eye ) != 0) {
      fprintf( stderr, "Error with matrix multiplication!\n" );
      printf( "Got:\n" );
      mat3_print( mmul );
      printf( "Expected:\n" );
      mat3_print( eye );
      return -1;
   }

   return 0;
}


static int test_translation (void)
{
   int i;
   double p[3], t[3], pf[3];
   dq_t P, T, TP, PF;

   /* Create positions. */
   p[0] = 1.;
   p[1] = 2.;
   p[2] = 3.;
   t[0] = 4.;
   t[1] = 5.;
   t[2] = 6.;
   for (i=0; i<3; i++)
      pf[i] = p[i] + t[i];

   /* Calculate. */
   dq_cr_point( P, p );
   dq_cr_translation_vector( T, t ); 
   dq_op_f4g( TP, T, P );
   dq_cr_point( PF, pf );

   /* Results. */
   if (dq_cmp( PF, TP ) != 0) {
      fprintf( stderr, "Translation failed!\n" );
      printf( "Point:\n" );
      dq_print_vert( P );
      printf( "Translation:\n" );
      dq_print_vert( T );
      printf( "Got:\n" );
      dq_print_vert( TP );
      printf( "Expected:\n" );
      dq_print_vert( PF );
      return -1;
   }

   return 0;
}


static int test_rotation (void)
{
   double a, p[3], s[3], c[3], pf[3];
   dq_t P, R, RP, PF;
   double norm;

   /* Create positions. */
   a    = M_PI/2.;
   p[0] = 1.;
   p[1] = 1.;
   p[2] = 1.;
   s[0] = 0.;
   s[1] = 0.;
   s[2] = 1.;
   c[0] = 0.;
   c[1] = 0.;
   c[2] = 0.;
   pf[0] = -1.;
   pf[1] = 1.;
   pf[2] = 1.;

   /* Calculate. */
   dq_cr_point( P, p ); /* B */
   dq_cr_rotation( R, a, s, c ); /* A */
   dq_op_f4g( RP, R, P ); /* ABA* */
   dq_cr_point( PF, pf );

   /* Check norm. */
   norm = dq_op_norm( R );
   if (fabs(norm - 1.) > 1e-10) {
      fprintf( stderr, "Rotation dual quaternion R is not a unit dual quaternion (has a norm of %.3f)\n", norm );
      dq_print_vert( R );
      return 1;
   }

   /* Results. */
   if (dq_cmp( PF, RP ) != 0) {
      fprintf( stderr, "Rotation failed!\n" );
      printf( "Point:\n" );
      dq_print_vert( P );
      printf( "Rotation:\n" );
      dq_print_vert( R );
      printf( "Got:\n" );
      dq_print_vert( RP );
      printf( "Expected:\n" );
      dq_print_vert( PF );
      return -1;
   }

   return 0;
}


static int test_movement (void)
{
   int i;
   double a, p[3], t[3], s[3], c[3], pf[3];
   dq_t P, R, T, RT, RTP, PF;
   double norm;

   /* Create positions. */
   a    = M_PI/2.;
   p[0] = 0.;
   p[1] = 0.;
   p[2] = 0.;
   s[0] = 1./M_SQRT2;
   s[1] = -1./M_SQRT2;
   s[2] = 0.;
   c[0] = 1.;
   c[1] = 0.;
   c[2] = 0.;
   t[0] = 1.;
   t[1] = 2.;
   t[2] = 3.;
   pf[0] = 0.5;
   pf[1] = 0.5;
   pf[2] = -1./M_SQRT2;
   for (i=0; i<3; i++)
      pf[i] = pf[i] + t[i];

   /* Calculate. */
   dq_cr_point( P, p ); /* B */
   dq_cr_rotation( R, a, s, c );
   dq_cr_translation_vector( T, t );
   dq_op_mul( RT, T, R ); /* A */
   dq_op_f4g( RTP, R, P );
   dq_op_f4g( RTP, RT, P ); /* ABA* */
   dq_cr_point( PF, pf );

   /* Check norm. */
   norm = dq_op_norm( R );
   if (fabs(norm - 1.) > 1e-10) {
      fprintf( stderr, "Rotation dual quaternion R is not a unit dual quaternion (has a norm of %.3f)\n", norm );
      dq_print_vert( R );
      return 1;
   }

   /* Results. */
   if (dq_cmp( PF, RTP ) != 0) {
      fprintf( stderr, "Rotation+Translation failed!\n" );
      printf( "Point:\n" );
      dq_print_vert( P );
      printf( "Rotation:\n" );
      dq_print_vert( R );
      printf( "Translation:\n" );
      dq_print_vert( T );
      printf( "Rotation+Translation:\n" );
      dq_print_vert( RT );
      printf( "Got:\n" );
      dq_print_vert( RTP );
      printf( "Expected:\n" );
      dq_print_vert( PF );
      return -1;
   }

   return 0;
}


static int test_homo (void)
{
   dq_t E, P, Q, PF;
   double R[3][3] = { { 0.36, 0.48, -0.8 },
                      { -0.8, 0.6, 0. },
                      { 0.48, 0.64, 0.6 } };
   double d[3] = { 1., 2., 3. };
   double p[3] = { 4., 5., 6. };
   double pf[3];

   /* Calculations with vector math. */
   mat3_mul_vec( pf, R, p );
   pf[0] += d[0];
   pf[1] += d[1];
   pf[2] += d[2];
   dq_cr_point( PF, pf );

   /* Calculations with quaternions. */
   dq_cr_homo( Q, R, d );
   dq_cr_point( P, p );
   dq_op_f4g( E, Q, P );

   /* Comparison. */
   if (dq_cmp( PF, E ) != 0) {
         fprintf( stderr, "Homogeneous matrix test failed!\n" );
         printf( "Got:\n" );
         dq_print_vert( E );
         printf( "Expected:\n" );
         dq_print_vert( PF );
         return -1;
   }
   return 0;
}


static int test_scara (void)
{
   int i;
   double s1[3] = { 0., 0., 1. };
   double c1[3] = { 0., 0., 0. };
   double s2[3] = { 0., 0., 1. };
   double c2[3] = { 300., 0., 0. };
   double s3[3] = { 0., 0., 1. };
   double c3[3] = { 0., -650., 0. };
   double s4[3] = { 0., 0., -1. };
   double a1, a2, a3, t4;
   dq_t S1, S2, S3, S4;
   dq_t S1234;
   dq_t St;

   /* Example taken form Alba Perez.
    *
    * SCARA robot Epson E2L65
    */
   for (i=0; i<10; i++) {
      /* Parameters. */
      a1 = 0.5+3.0*(10./(double)(i+1));
      a2 = 1.0-1.0*(10./(double)(i+1));
      a3 = 0.0+1.5*(10./(double)(i+1));
      t4 = (double)(i);
      /* Create individual rotations. */
      dq_cr_rotation_plucker( S1, a1, s1, c1 );
      dq_cr_rotation( S2, a2, s2, c2 ); /* We spice it up by using non-plucker here. */
      dq_cr_rotation_plucker( S3, a3, s3, c3 );
      dq_cr_translation( S4, t4, s4 );
      /* Create movement. */
      dq_op_mul( S1234, S1, S2 );
      dq_op_mul( S1234, S1234, S3 );
      dq_op_mul( S1234, S1234, S4 );
      /* Calculate movement as per the document. */
      St[0] = cos( (a1+a2+a3)/2. );
      St[1] = 0.;
      St[2] = 0.;
      St[3] = sin( (a1+a2+a3)/2. );
      St[4] = 25.*( 6.*cos((a1-a2-a3)/2.) + 7.*cos((a1+a2-a3)/2.) - 13.*cos((a1+a2+a3)/2.));
      St[5] = 25.*( 6.*sin((a1-a2-a3)/2.) + 7.*sin((a1+a2-a3)/2.) - 13.*sin((a1+a2+a3)/2.));
      St[6] = -t4/2.*cos((a1+a2+a3)/2.);
      St[7] =  t4/2.*sin((a1+a2+a3)/2.);
      /* Compare with real movement. */
      if (dq_cmp( S1234, St ) != 0) {
         fprintf( stderr, "Scara test failed!\n" );
         printf( "Got:\n" );
         dq_print_vert( S1234 );
         printf( "Expected:\n" );
         dq_print_vert( St );
         return -1;
      }
   }
   return 0;
}


static int test_benchmark (void)
{
   int i, N;
   double a, p[3], s[3], c[3], pf[3];
   dq_t P, R, RR, RP, PF, PS, PA;
   struct timeval tstart, tend;
   unsigned long elapsed;
   double dt;
   double n, na, ns;
   double norm;

   N = 1000000;

   /* Create positions. */
   a    = M_PI/2./(double)N;
   p[0] = 1.;
   p[1] = 1.;
   p[2] = 1.;
   s[0] = 0.;
   s[1] = 0.;
   s[2] = 1.;
   c[0] = 0.;
   c[1] = 0.;
   c[2] = 0.;
   pf[0] = -1.;
   pf[1] = 1.;
   pf[2] = 1.;

   /* Calculate. */
   dq_cr_point( P, p ); /* B */
   dq_cr_rotation( R, a, s, c ); /* A */
   dq_cr_copy( RR, R );
   gettimeofday( &tstart, NULL );
   for (i=0; i<N; i++)
      dq_op_mul( RR, RR, R );
   gettimeofday( &tend, NULL );
   dq_cr_copy( R, RR );
   dq_op_f4g( RP, R, P ); /* ABA* */
   dq_cr_point( PF, pf );

   /* Check norm. */
   norm = dq_op_norm( R );
   if (fabs(norm - 1.) > 1e-10) {
      fprintf( stderr, "Rotation dual quaternion R is not a unit dual quaternion (has a norm of %.3f)\n", norm );
      dq_print_vert( R );
      return 1;
   }

   /* Display elapsed time. */
   elapsed = ((tend.tv_sec - tstart.tv_sec) * 1000000 + (tend.tv_usec - tstart.tv_usec));
   dt      = ((double)elapsed) / 1e6;
   fprintf( stdout, "Benchmarked %d multiplications in %.3f seconds (%.3e seconds/multiplication).\n", N, dt, dt/(double)N );

   /* Compare result. */
   dq_op_add( PA, PF, P );
   dq_op_sub( PS, PF, P );
   na = dq_op_norm( PA );
   ns = dq_op_norm( PS );
   n = (na > ns) ? ns : na;
   if (n > 1e-10) {
      fprintf( stderr, "Error with %d multiplications is above 1e-10: %.3e\n", N, n );
      return -1;
   }

   return 0;
}


static int test_inversion (void)
{
   dq_t Q, Qinv, M, I;
   double a, s[3], c[3], z[3] = { 0., 0., 0. };

   a    = M_PI_2;
   s[0] = 1.;
   s[1] = 1.;
   s[2] = 1.;
   vec3_normalize( s );
   c[0] = 2.;
   c[1] = 3.;
   c[2] = 4.;

   dq_cr_rotation( Q, a, s, c );
   dq_cr_inv( Qinv, Q );
   dq_op_mul( M, Q, Qinv );
   dq_cr_translation_vector( I, z );

   printf("normQ:    %f\n", dq_op_norm(Q) );
   printf("normQinv: %f\n", dq_op_norm(Qinv) );
   printf("normM:    %f\n", dq_op_norm(M) );
   printf("Q:\n");
   dq_print_vert( Q );
   printf("Qinv:\n");
   dq_print_vert( Qinv );
   printf("I:\n");
   dq_print_vert( I );
   printf("M:\n");
   dq_print_vert( M );

   if (dq_cmp( M, I ) != 0) {
      fprintf( stderr, "Failed dual quaternion inversion test!\n" );
      printf( "Got:\n" );
      dq_print_vert( M );
      printf( "Expected:\n" );
      dq_print_vert( I );
      return -1;
   }

   return 0;
}


int main( int argc, char *argv[] )
{
   int ret;

   /* Warnings. */
   (void) argc;
   (void) argv;

   ret  = 0;
   ret += !!test_vector();
   ret += !!test_matrix();
   ret += !!test_translation();
   ret += !!test_rotation();
   ret += !!test_movement();
   ret += !!test_homo();
   ret += !!test_scara();
   ret += !!test_inversion();
   ret += !!test_benchmark();

   if (ret == 0) {
      fprintf( stdout, "All tests passed.\n" );
   }

   return ret;
}


