#include "gtest/gtest.h"
#include "evt/gev_distribution.hpp"
#include "evt/gpd_distribution.hpp"
#include "evt/evtapproach_bm.hpp"
#include "evt/evtapproach_pot.hpp"
#include "statistical/estimator_pwm.hpp"
#include "utility/oop.hpp"

using namespace chronovise;

class EstimatorPWM_Test : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    EstimatorPWM_Test() 
    {
        // You can do set-up work for each test here.
    }

    virtual ~EstimatorPWM_Test() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:
    virtual void SetUp() {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    virtual void TearDown() {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }


};

TEST_F(EstimatorPWM_Test, GPD_0_1_0) {

    constexpr int n_elem = 200;
    double xs[n_elem] =
    {
1.819034,0.230314,1.167271,0.637650,1.797886,0.507528,1.335710,0.424527,0.372203,0.290150,0.797305,2.479067,1.474134,0.090650,1.881391,0.191382,0.619260,0.003873,2.548799,0.814912,2.238177,0.038847,5.374286,0.255008,0.201745,0.140764,2.471763,0.916834,1.347572,0.223058,0.840687,0.093599,1.704589,1.332553,1.927311,1.994596,0.140076,0.545237,0.598091,1.931333,0.158959,0.474727,1.047105,0.666993,0.911781,2.577460,1.427466,2.092981,1.693321,1.427314,0.874029,3.002668,0.102347,0.056796,0.711588,0.714876,1.085540,0.105301,0.996290,2.196400,0.248138,0.942278,1.420094,0.906558,2.338684,2.025156,0.059696,0.044857,0.553023,2.817092,1.449107,1.040838,0.196996,4.173165,3.146002,1.777916,0.432145,0.312354,0.434257,0.796457,0.603290,1.216313,0.294783,1.666246,0.375748,1.695480,0.998356,0.469014,0.248170,2.511755,0.073231,0.253973,0.719919,0.830437,0.805681,1.183029,0.676273,0.671833,0.201348,0.229625,0.439563,0.971250,0.208772,0.629561,1.047747,0.062938,0.132454,0.597553,0.474052,0.532654,1.571457,1.199827,0.753060,1.467556,0.169237,1.635965,1.487566,1.767801,1.479883,0.830804,1.167634,0.079715,0.843488,1.688393,0.099952,0.020459,0.823552,2.197152,1.354545,0.894725,0.519369,1.338603,0.506098,0.340779,1.506219,2.142018,1.215115,1.143259,0.857629,0.677553,2.459054,1.337572,0.221876,3.532892,0.073804,0.314258,0.716193,0.547273,1.438499,0.779034,0.037610,0.603662,0.651745,1.462768,0.715602,0.471509,0.386935,0.927566,1.001204,0.012091,3.277065,0.121978,0.090705,0.227925,2.315546,1.339903,1.092560,0.386063,1.991041,0.326801,2.237155,0.425019,0.704868,0.249678,0.335421,0.101235,0.115498,1.096126,0.358468,1.620449,3.488687,0.295614,0.693102,0.734131,0.100127,0.494515,0.481807,0.151472,0.216305,0.550396,1.698693,1.427400,0.120461,3.551759,0.713551,1.784225,0.021550,0.338702,0.692204, 0.693211};

    MeasuresPool<int, double> mp;
    for (int i=0; i<n_elem; i++) {
        mp.push(0, xs[i]);
    }

    Estimator_PWM<int, double> mle;
    mle.set_source_evt_approach(&typeid(EVTApproach_PoT<int, double>));
    mle.run(mp);
    auto res = mle.get_result();

    EXPECT_NEAR(std::dynamic_pointer_cast<GPD_Distribution>(res)->get_location(), 0., 5e-1);    
    EXPECT_NEAR(std::dynamic_pointer_cast<GPD_Distribution>(res)->get_scale(), 1, 5e-1);
    EXPECT_NEAR(std::dynamic_pointer_cast<GPD_Distribution>(res)->get_shape(), 0., 5e-1);

}



TEST_F(EstimatorPWM_Test, GPD_2_20_1) {

    constexpr int n_elem = 200;
    double xs[n_elem] =
    {
2.972253,12.615112,7.854437,12.186839,14.686876,9.724292,8.931033,17.720373,10.825809,2.873886,4.324948,2.573393,9.363876,13.881676,13.124022,21.863925,15.303356,6.609077,9.077864,13.039553,3.194545,2.428168,12.156383,13.166223,6.046873,19.590124,5.452777,7.074252,10.263555,15.940262,10.298249,10.502486,17.870576,20.857918,17.409326,2.219657,5.872224,21.140228,11.152038,16.706098,11.817679,10.357776,21.088323,4.483723,12.320992,18.115788,2.373304,3.947349,9.443776,14.620806,3.432844,4.029801,20.958898,20.237815,17.227604,15.312286,4.914226,21.698486,16.830726,7.574872,20.229929,4.759897,18.666830,12.968636,16.324473,11.045289,2.571211,19.024144,4.633637,12.231589,10.181876,21.932225,12.288877,7.371050,11.260571,2.269745,11.968961,21.005518,19.790748,12.570655,18.697562,12.889600,9.662577,19.700337,20.348584,21.619802,20.255484,13.717492,13.159500,4.875694,17.189338,5.132981,14.503189,21.619719,7.077687,18.711226,21.600973,10.293159,6.018056,8.367920,16.037691,11.298308,18.505867,11.354837,17.756975,8.312181,14.236784,17.408095,10.497336,19.384572,16.302854,20.472004,14.213144,4.590434,9.765381,14.664925,9.097005,9.658657,15.246266,18.689049,3.100474,11.800784,2.925731,12.601912,3.341977,6.165653,12.114481,21.726561,17.195699,13.242380,4.240435,3.735613,9.526876,12.615748,5.516989,14.747101,8.664883,15.034160,19.524621,12.671436,21.198630,11.065567,2.743819,18.308146,21.159540,13.087160,14.085170,2.596475,8.612978,7.396533,19.806913,12.100668,3.491440,8.557256,13.532752,15.623421,2.164351,6.911866,15.488838,14.436164,10.631625,7.352881,13.800982,9.726953,2.559820,4.876822,14.302165,16.537489,5.310755,12.155094,12.585103,19.095248,19.994198,15.940793,2.330914,20.446230,16.749579,14.743267,9.133148,20.073035,21.506312,15.766427,2.915881,3.894031,7.432066,8.885687,9.675695,17.727717,7.845240,18.250215,4.123982,20.837789,15.539371,4.930078,19.568696,6.999093,12.546647,15.949105,9.953557,17.752163
};
    
    MeasuresPool<int, double> mp;
    for (int i=0; i<n_elem; i++) {
        mp.push(0, xs[i]);
    }

    Estimator_PWM<int, double> mle;
    mle.set_source_evt_approach(&typeid(EVTApproach_PoT<int, double>));
    mle.run(mp);
    auto res = mle.get_result();

    EXPECT_NEAR(std::dynamic_pointer_cast<GPD_Distribution>(res)->get_location(), 2., 5e-1);    
    EXPECT_NEAR(std::dynamic_pointer_cast<GPD_Distribution>(res)->get_scale(), 20., 5e-1);
    EXPECT_NEAR(std::dynamic_pointer_cast<GPD_Distribution>(res)->get_shape(), -1., 5e-1);

}


TEST_F(EstimatorPWM_Test, GEV_M05_1_2) {

    constexpr int n_elem = 1000;
    double xs[n_elem] =
    {

1.583457,2.902126,3.313949,3.222706,2.563704,2.698465,3.403960,2.170689,1.774062,2.644932,1.791515,2.621930,0.509266,3.852967,1.489083,2.587729,1.944164,2.848215,0.218048,0.710363,3.449609,0.880375,1.901028,2.397364,1.633723,2.436886,3.005315,2.387160,3.469053,1.231202,2.128924,1.744411,2.566129,0.360697,0.681095,1.873532,0.954392,1.337900,2.008908,0.408425,2.827720,2.667043,1.355496,0.666429,2.102866,2.245130,1.473219,3.252258,2.565756,0.115841,3.348853,2.220132,0.625471,1.005486,3.920604,3.242392,2.606015,0.197908,1.872483,2.246885,0.958644,2.502820,1.882227,2.214039,2.518850,0.779848,0.618456,1.808389,2.386777,2.481492,1.616172,3.395596,3.124826,2.199220,3.730926,2.519840,1.591161,3.083502,2.215982,1.645075,3.572983,1.209044,2.364970,3.662500,2.683383,2.604768,2.261303,2.518702,3.390513,2.022547,1.570958,2.146201,1.723071,2.199270,2.634734,2.417395,2.047193,3.264325,1.806545,2.085616,2.377269,0.663264,1.580290,1.079068,0.957067,3.318166,0.312439,3.162139,2.352313,1.051070,2.311876,2.569257,0.898035,3.703240,2.694398,2.240374,3.234592,1.688073,3.119932,1.891187,3.508094,1.625010,3.581255,2.360819,2.487818,3.841208,2.980024,1.846869,0.623901,0.466226,3.089850,2.116860,2.043792,2.390116,3.324361,2.100950,2.580793,0.930180,1.911268,1.219991,1.627672,2.029930,1.712512,1.522636,2.649251,3.054486,1.495804,3.338097,2.916636,2.421518,3.650743,2.488529,1.530438,3.216605,3.229284,1.845389,1.885438,3.164925,2.316914,0.560007,2.997992,2.154206,1.743364,1.904206,1.997445,3.041513,0.393254,2.869896,3.261945,1.761776,2.703388,1.582144,2.621756,0.782163,3.632254,2.593806,2.042599,0.264900,3.216016,2.578821,3.187441,2.345966,-0.399783,3.419002,2.113330,2.883801,1.250616,3.096051,3.384378,2.997797,2.945051,2.099073,2.810105,2.489882,2.534948,1.936548,2.815693,1.296522,-1.096290,2.481775,2.445330,2.455548,2.792506,3.451297,3.520808,3.362062,2.580134,3.446269,1.070126,1.924953,0.364536,2.426203,2.895723,1.386168,2.153125,0.952985,1.815242,3.003397,2.422875,2.975166,2.661435,3.327683,0.652183,1.362903,2.127811,2.902027,3.327038,0.176223,1.183141,2.147633,2.963765,2.393227,2.938445,1.336812,2.740769,2.614055,-0.465010,2.903095,3.817804,1.133428,1.980066,1.426987,2.840146,1.371906,3.769883,-0.037124,0.408913,3.066737,3.222660,2.491736,2.937525,2.381532,2.537675,1.425603,2.343150,0.548852,0.605478,1.909033,2.642649,3.323793,2.742350,2.770453,2.795388,3.054793,2.712213,2.382535,1.899129,3.478104,1.633157,2.361730,2.903322,1.659267,3.180911,2.425694,3.386286,1.952752,3.482849,3.452183,2.541867,0.976679,1.997270,1.730274,1.698789,1.562920,-1.149576,3.327213,2.221500,2.520198,1.852199,3.845067,3.745141,3.620022,2.725532,2.868651,1.905735,2.389275,1.722294,2.849769,2.997990,0.829818,1.544825,1.478228,2.626762,2.866332,3.148918,0.015612,1.471043,2.259938,2.028385,1.923799,0.682652,2.955623,2.092481,2.744460,2.456131,0.555000,1.828094,3.624946,3.467137,2.027397,2.611763,2.483023,3.138248,3.584202,0.785316,3.310780,2.482821,1.440259,1.544357,2.819363,2.563006,2.538589,3.642157,2.527960,0.963829,1.322581,0.978832,1.226946,2.738026,2.663562,2.012347,1.305721,2.057998,3.055177,2.085000,2.940038,1.790867,2.663919,3.301329,1.500122,2.287427,1.046712,1.155949,0.684077,0.815946,2.633425,2.114954,2.660607,3.213020,2.961263,3.689333,3.007040,3.483918,2.900452,1.657421,2.851878,2.790065,2.985971,2.920371,1.860376,2.359878,2.985327,2.507095,3.565266,1.344628,3.375969,2.932803,1.762903,2.634490,2.244551,1.140249,2.452155,3.648061,2.193204,2.549482,1.034537,1.462145,1.306136,0.366734,1.720270,1.575875,1.974461,1.166372,3.924996,2.367254,2.053571,1.644680,1.991482,2.083076,3.446624,2.324002,2.591980,-0.620553,2.541933,2.996566,2.704172,2.402423,3.124934,3.608643,1.846675,3.047542,1.759777,-0.102067,0.925675,1.890460,1.822305,-0.008069,1.302683,2.197830,2.969334,2.760940,2.840481,2.237042,3.419734,3.789115,3.471691,2.241338,3.367841,2.049873,2.577593,2.477832,3.181647,1.758616,2.718594,2.575824,2.702481,1.835379,1.898863,1.415226,0.969637,1.764117,1.964193,2.420043,3.808070,0.221529,2.828420,3.368662,3.240876,1.083008,3.572188,2.190099,3.271541,3.238208,1.965091,2.643214,3.236988,0.068778,0.796073,2.023875,1.241370,0.323834,3.008877,1.886192,3.102151,1.353575,2.748780,3.271041,2.945132,1.574053,1.975559,1.987141,1.703177,1.915031,0.874428,2.216691,2.224391,0.237429,2.657129,0.639957,1.334144,2.769059,2.464335,-0.521080,1.768858,2.023598,1.228158,0.777265,2.239415,2.006825,3.121573,2.424233,3.102259,2.238522,-0.130998,-0.605076,1.320894,1.994718,2.849361,1.289878,1.739590,2.675462,1.766114,1.870758,1.271158,2.053086,3.336335,3.309939,2.068797,2.747220,1.652681,3.546982,2.628122,1.489790,1.026507,2.490792,1.725435,0.748692,1.285358,0.532798,2.760059,3.007603,3.074842,1.695376,3.336895,2.589180,-0.118480,1.934057,2.415087,2.635312,2.204690,3.084889,1.219922,3.680671,3.147217,1.922971,2.608425,1.816856,0.890875,2.386313,3.123602,2.962149,3.536184,1.904318,2.058578,1.247277,1.904768,2.462248,2.454293,1.294485,1.070961,2.081750,3.140547,1.402006,2.336778,1.123210,3.237228,2.969019,2.487288,2.058131,2.311132,3.307126,3.368282,2.331019,2.404584,3.384729,2.520613,2.992270,2.714069,2.261320,1.538143,2.576597,1.398913,1.452870,3.229262,1.119541,2.676964,2.181106,2.599950,2.896346,1.815417,0.451870,3.152142,2.003880,2.713939,3.336291,1.724684,3.908267,3.148887,3.032482,2.702832,2.438709,2.050483,3.115167,2.559678,3.007308,3.777271,3.024428,3.202282,2.300184,3.265278,1.905176,1.483494,2.321313,0.275997,3.129928,1.692767,2.752047,3.039078,2.762334,2.689099,1.601455,2.280240,3.487391,1.614604,1.498100,1.719457,2.992353,1.898196,2.577728,1.397894,0.878086,1.832340,1.578543,3.382896,3.489332,0.287420,2.555742,0.462927,2.149708,2.386445,3.165855,2.628933,1.662746,3.367034,2.970677,2.483333,3.341127,2.000543,1.916248,2.614527,0.873948,2.020603,1.411865,3.083330,2.861575,2.745368,3.400183,3.111676,2.450602,-0.081292,0.929374,2.370205,3.558714,3.173645,3.481558,3.671691,2.012592,1.867749,3.011454,1.509921,3.591561,3.462151,1.994147,1.936542,2.788084,2.845475,3.056148,2.555797,0.946319,2.475933,0.453472,2.946325,2.709962,2.569298,1.564571,2.838497,2.864623,1.934589,0.301986,3.175238,2.978286,2.358004,2.822245,3.032038,1.896063,3.399067,2.516188,1.768310,1.701294,3.317846,1.995160,1.245355,3.530249,2.689277,3.732078,1.438161,3.133177,2.319976,2.549050,0.476634,2.487533,2.648815,2.330324,0.641606,2.371170,0.574337,3.175284,2.558135,2.784125,2.680821,2.625223,3.565304,1.367950,2.274149,3.171758,2.327590,2.958313,1.380473,2.785561,3.401803,1.304738,2.157819,0.437069,1.560046,2.725261,3.257794,1.936025,0.203873,3.369102,2.264759,3.387461,1.888315,2.784326,-0.024193,2.364453,1.899351,1.911642,3.562837,2.928537,1.905396,2.647152,2.125793,2.515709,3.593343,2.607513,1.248074,3.159966,2.868669,1.021531,1.298763,1.796377,2.146347,2.402659,1.727882,3.130989,3.036250,1.723558,2.607155,0.975304,2.734247,1.943510,2.733145,1.791149,2.758979,2.069217,2.064069,1.489850,2.864667,3.107143,3.447291,2.368683,1.435377,2.577323,1.455533,2.112524,3.202735,2.160318,3.444123,2.014081,2.801362,2.544026,0.970834,2.117689,3.440439,0.599728,2.043484,1.612991,1.567512,1.970535,3.438102,2.340888,3.063828,1.951542,2.234331,3.552163,1.159204,0.741239,2.820128,3.468055,1.021868,2.691297,1.226033,2.791138,2.630296,0.461711,3.049753,3.764329,2.179664,1.936082,0.524428,1.558072,2.526253,2.573766,1.653708,2.697501,0.646304,3.504673,0.914410,1.905550,2.689780,2.344275,2.408605,2.030337,2.974749,1.751412,2.685614,3.430752,1.896072,3.091189,3.022821,1.112180,1.280409,2.124387,2.775359,2.705395,2.757912,-0.321326,1.135756,2.962098,3.170360,2.919706,3.522194,3.421408,1.550306,2.358292,3.023093,2.388513,2.611114,2.472957,2.880442,0.678399,2.317527,1.575124,1.915282,2.457453,2.911631,2.117459,2.353154,1.137723,1.519773,1.783549,2.212730,1.611744,1.689502,3.322483,2.538225,3.496864,2.612718,1.647620,0.981704,2.445047,2.194469,2.785285,2.950858,2.084026,2.494454,1.484527,3.517803,2.365550,1.731554,1.572306,3.560839,2.780322,2.824430,2.703559,3.000428,2.524984,2.342643,3.280900,2.067049,1.619920,1.748624,3.367312,2.811959,3.202678,2.029159,2.647594,0.281955,3.422609,2.261539,2.382455,2.968175,1.911939,2.631163,1.885200,1.838872,2.678131,2.171972,0.983957,1.597265,3.661634,2.802918,2.709498,1.882870,0.039714,2.501614,0.358585,1.694358,1.425299,2.211073,2.974110,2.635717,1.849526,1.781123,0.807509,1.346257,2.856799,0.892531,3.414295,2.161825,0.819309,2.505524,3.141638,1.943429,3.002797,2.825552,2.549432,2.024625,2.069354,2.158359,2.144929,1.535022,2.327112,0.409937,2.488373,0.528388,3.260694,3.223356,1.917745,3.675576,3.569746,3.786045,1.281950,-0.831868,1.638982,1.940409,1.587313,2.941968,1.560705,2.208941,1.676888,1.939019,3.360993,-0.179036,1.155168,2.054144,3.166659,1.513868,3.482475,3.649771,2.019960,3.752995,1.099633,2.954672,2.522396,2.619895,2.310568,1.540665,1.216470,2.652627,3.061643,2.733493,2.266082,3.521993,2.550689,1.563677,2.767075,1.866995,2.924593,3.913779,1.710615,3.468729,2.796921,0.933306,1.631082,3.480096,2.286035,3.292992,3.001276,-0.000868

    };

    
    MeasuresPool<int, double> mp;
    for (int i=0; i<n_elem; i++) {
        mp.push(0, xs[i]);
    }

    Estimator_PWM<int, double> mle;
    mle.set_source_evt_approach(&typeid(EVTApproach_BM<int, double>));
    mle.run(mp);
    auto res = mle.get_result();

    ASSERT_TRUE(instanceof_ptr<GEV_Distribution>(res));

    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_location(), 2, 5e-1);    
    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_scale(), 1., 5e-1);
    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_shape(), -0.5, 5e-1);
    EXPECT_LT(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_shape(), 0);

}


TEST_F(EstimatorPWM_Test, GEV_02_05_1) {

    constexpr int n_elem = 1000;
    double xs[n_elem] =
    {

1.419847,0.982652,6.488776,0.806704,1.463811,1.368834,1.026472,0.687288,0.426141,1.073661,0.750328,1.638849,1.003377,2.052902,1.662038,1.307066,0.739799,3.178870,0.862535,2.659685,0.806151,1.007738,0.592187,1.438005,0.745282,0.493742,1.631862,0.972356,1.481385,1.021833,1.412159,0.410899,2.514269,1.876722,1.695193,1.926053,1.021062,1.392361,1.314943,1.237885,0.875598,0.840049,1.117463,0.811570,1.891638,4.371601,0.445168,1.247063,0.591356,1.882559,4.670775,0.549005,2.852535,0.393930,1.533721,1.815699,1.244558,2.309237,2.412886,1.409359,0.680436,0.797911,0.747493,0.484304,0.628463,1.390739,2.856438,0.981818,1.058914,4.232575,2.950083,1.517113,4.578678,1.759349,0.957928,1.485238,0.834005,0.902895,1.525225,1.234304,1.060265,1.364407,1.708980,1.329294,1.273943,1.329361,1.208652,0.582411,1.622296,6.100088,0.981924,3.569623,0.971025,2.317657,1.121927,1.062837,0.797815,0.660574,0.920780,1.639738,1.812705,1.557323,0.340412,2.061043,2.634319,1.772677,0.486799,1.014049,1.583214,1.649008,0.806849,0.867542,1.508908,1.155747,1.404963,0.823510,0.740192,1.996903,1.759639,2.782886,0.630147,0.747619,0.614242,1.174366,0.763448,2.387873,0.614232,0.491198,1.283295,1.777724,0.924817,0.742904,0.960955,0.787271,1.206021,2.475731,1.415306,0.618717,1.031443,0.519475,1.192134,1.088730,6.824697,1.919928,1.168092,2.376565,0.679921,1.030271,2.692235,2.582535,1.606656,1.394419,0.966774,2.804134,0.659112,1.651949,1.451206,2.012924,1.041712,1.706904,2.022547,0.938866,1.274736,3.909007,1.269752,0.949518,1.396631,0.990175,1.727057,1.063502,1.178322,1.559630,3.624099,0.945946,2.034753,1.675669,3.109876,0.452164,0.985078,1.485833,0.884198,0.815230,1.600374,1.406655,1.342128,1.480996,0.500772,0.974170,1.117028,0.829579,1.610462,2.128452,0.884207,1.653230,0.680267,2.029621,0.681604,1.337754,0.997660,1.900648,1.196026,1.174107,2.252823,0.980044,1.114266,3.329977,0.485727,3.632695,0.757676,1.495678,1.334542,1.513623,0.990697,1.398208,1.918107,0.399432,0.584991,3.706665,1.461471,0.816400,1.048948,0.654511,0.866716,0.852486,0.951181,0.702929,0.973124,0.653903,2.300762,0.605243,2.725130,1.042735,0.500348,0.965545,1.666896,1.854788,1.262371,1.539310,2.370259,0.519920,0.913772,0.496973,0.766623,1.623869,1.628076,2.257734,1.327315,0.557341,2.638908,1.876015,0.890136,1.260298,4.264614,1.612106,2.040337,1.090929,1.145459,1.289154,0.867591,1.704520,1.196193,1.451901,0.919220,0.681800,1.152863,0.992644,1.831066,1.803854,1.498773,0.673423,0.410466,1.287655,0.909981,2.852703,4.006984,0.891036,1.877715,2.389611,1.354867,2.299811,2.919610,1.269499,1.645931,1.317183,0.429118,1.110036,1.450839,1.223590,1.006033,2.819696,1.996404,2.090755,1.006335,1.346849,2.224090,2.769789,1.498666,0.782561,1.466795,0.560329,1.053459,1.495259,2.772774,1.917299,1.166415,1.727790,1.067926,3.588703,4.544995,2.173351,1.028727,1.121996,0.837422,1.818088,2.291636,2.544455,1.284988,1.357360,0.697746,2.418703,1.115648,0.781013,2.418188,1.745885,2.289219,0.888805,1.509348,1.489402,0.655843,1.054284,0.875888,1.614688,0.886714,2.390303,1.983207,1.030301,1.186890,1.559780,2.018570,1.377636,1.313603,0.943655,1.124461,1.607228,2.302524,1.625694,0.396174,1.512860,1.098454,1.097464,0.646069,1.932473,0.942068,0.836800,0.966002,1.010642,1.265126,1.291232,1.038305,1.041502,1.214273,1.474695,3.045419,1.629660,1.044206,2.007033,0.674771,0.533916,0.585739,0.720628,0.941218,0.911192,0.354693,1.254057,0.607307,0.694074,1.419751,2.145797,3.682713,1.306737,6.409460,1.276905,1.214418,0.949864,1.086277,1.177495,0.558115,2.326258,0.543718,1.095117,1.983431,1.036524,1.385004,1.948945,2.315452,2.738668,0.759935,0.853477,2.403631,1.347175,1.196119,1.383725,1.952244,1.240880,0.775957,1.120755,1.083300,3.400986,1.397771,1.561196,1.623865,0.971626,1.216856,1.282270,0.709448,1.291466,1.559775,1.081231,2.027483,1.654156,0.989355,1.121221,1.025296,1.787856,1.662153,1.086670,1.557238,2.943956,1.817424,1.586299,0.632702,1.030169,1.342669,1.128801,0.508335,0.812907,2.017732,0.380057,2.170813,0.573130,1.499955,1.190468,0.798177,1.308102,0.654794,1.504709,1.358697,0.522908,0.523826,0.703339,0.401236,1.093670,2.008639,1.392576,1.221869,2.171727,0.611654,2.490952,0.630373,1.216866,0.688814,1.286848,0.285190,1.758871,2.088813,2.575617,4.447995,1.198139,0.870713,0.617285,1.202396,1.333039,1.746833,0.583158,1.483520,1.216838,0.731265,2.840177,1.341898,1.101513,2.890838,1.471214,1.117910,2.043844,1.242080,1.277491,1.524964,0.999063,0.827385,1.321035,2.189487,1.053528,0.638446,1.106148,0.909134,1.046021,2.013902,1.049140,1.030507,0.989921,0.684232,0.855559,0.590839,1.085416,0.851727,0.905628,1.078964,0.649765,1.182509,1.588398,0.833202,1.820346,0.564726,1.035623,0.265969,0.801887,0.211755,0.757637,0.687756,0.866229,0.736924,0.681680,1.357393,2.429871,2.852259,0.802587,1.163568,1.011077,1.227732,0.861931,0.552181,1.095321,0.735399,0.430134,3.120322,1.087125,3.278408,1.745348,0.318304,1.524902,1.587250,1.448388,1.274818,0.798337,1.777301,0.812003,1.004061,2.349733,2.129515,1.047477,0.932933,1.375742,2.510750,2.500548,1.343931,0.952395,2.111634,1.104056,2.458025,0.456571,1.241758,1.614239,0.743369,0.957705,0.755532,0.938154,1.049457,1.268503,0.504018,1.275533,0.875252,0.830390,0.832621,0.705883,3.157439,2.799060,1.949255,1.645591,0.738273,0.989815,0.757078,0.207571,0.930797,1.571513,1.408006,1.259297,1.099214,0.892113,1.192719,1.742628,1.745328,1.315937,1.700517,1.449235,0.656520,1.196990,0.972119,0.601191,0.696152,0.770442,1.507192,1.088430,1.558810,0.851056,0.340057,1.241524,0.881379,2.961111,2.476436,1.033968,0.424972,1.505317,2.031743,3.578287,0.525295,1.115547,1.327382,1.540286,1.621934,1.458700,1.641930,1.008125,1.325788,0.644497,0.527080,3.942946,0.888636,1.350144,3.293826,0.752749,0.763155,0.964565,2.761806,1.031185,0.873117,0.702488,1.040086,1.009318,0.669546,1.093477,0.599975,1.387225,0.349570,1.310996,1.836829,0.822040,1.112196,1.304145,0.536147,1.184395,1.442543,0.802699,2.031202,3.563076,2.076871,1.199495,0.880688,1.697446,0.824172,3.178001,1.398172,1.359959,0.733561,0.597730,0.849005,2.141613,2.518962,1.571554,1.637250,0.814549,1.315933,1.916005,1.049439,4.593102,0.597057,0.936836,1.208003,0.534250,1.638613,1.282033,1.236760,1.998435,2.142662,1.834324,0.932684,1.118292,1.714087,0.633631,0.633421,0.868653,1.229114,3.620949,1.598535,0.924710,0.897492,2.097362,2.524480,1.436278,0.849151,0.594467,2.036913,1.331430,2.993836,0.535260,1.331291,0.889016,1.988329,0.760224,1.104247,1.034971,1.983185,1.517630,0.783718,0.933047,0.673920,1.505375,1.307005,0.729370,0.695851,1.153624,2.491410,1.274590,0.455739,0.517550,1.894020,1.117078,1.020156,1.836520,0.995122,1.241633,1.601726,2.217385,0.947197,1.458863,3.708109,0.568721,1.335593,1.063482,0.921076,0.860537,1.733991,5.773999,0.753891,1.806760,0.767079,5.121811,1.883210,1.078069,1.647233,1.187587,1.909461,0.984591,0.562909,1.342827,2.510681,0.764189,1.089653,1.704938,0.476265,2.962730,1.749310,1.285905,0.749956,1.186961,1.218219,5.508907,2.121226,3.300126,1.522374,1.048963,2.789690,1.158749,0.817157,1.038952,1.585060,1.285457,1.727428,5.857598,3.300841,1.246082,3.339199,0.643651,0.511274,0.914689,1.323299,1.239372,2.431130,1.255125,1.089100,1.258639,1.603671,0.385877,1.878097,0.687795,1.157226,0.851125,1.001648,1.483889,0.729136,0.880566,0.770516,0.766047,0.944722,2.274612,1.146171,1.049614,0.743267,3.489883,1.054476,2.067382,1.388574,1.011897,2.253691,1.819585,1.137030,1.929581,2.408316,1.085189,0.954750,1.353237,2.437725,1.577558,1.013050,1.664065,3.108401,1.258882,1.254389,0.923709,0.558547,0.747261,0.602796,1.134862,0.336528,2.557463,1.443426,0.216283,0.446617,0.784931,1.122324,0.663238,0.330597,1.642377,0.981360,1.804367,1.095794,1.095648,0.505306,0.506437,0.599182,1.348417,0.829823,2.051968,2.134100,3.332191,1.173045,0.801380,0.809505,1.248911,1.744392,0.972531,1.131529,1.436376,2.580906,0.717135,1.611995,1.318925,1.090984,2.301389,1.034475,0.742894,1.424171,1.405524,0.946196,1.885910,9.833387,4.011335,0.662860,0.817770,0.419758,1.373456,0.635294,1.054481,2.300230,1.267775,1.001528,0.784757,1.101958,3.152641,0.657866,1.145665,2.132361,0.488945,1.552128,3.901512,0.886558,0.673870,1.537093,2.503846,1.380004,2.420926,0.763716,1.720711,0.970772,1.070150,0.708267,1.950462,1.407349,1.674216,1.894211,0.549630,3.043051,1.186385,1.722900,1.685206,2.003644,0.709459,1.125758,1.393958,2.752465,2.021928,2.384188,1.327469,1.327879,2.121628,0.462317,2.309665,1.054860,0.467333,1.696071,0.706907,0.689996,1.370664,0.847952,0.941131,1.046583,1.052965,1.025023,1.377965,0.725098,0.756077,0.605906,0.939837,1.768261,0.820336,1.679354,1.554990,1.972229,1.989424,0.900041,0.921387,1.226524,0.942661,2.006901,1.914669,1.282788,0.859368,1.526120,0.819701,1.124462,1.022792,1.251901,5.013311,1.723127,3.981209,0.821243,1.235459,0.511242,1.728177,1.363172,2.133858,4.576055,2.718195,1.057354,0.150117,1.255669,0.783896,0.799962,0.943340,0.608390,1.700137,1.703013,1.259692,0.959850,2.009154,1.275262,3.182449,2.364129,0.984584,1.264872,0.971339,1.403163,1.861962,1.695273,0.660378,1.964933,0.426205,1.064243,1.654212,1.807545,0.999193
    };
    
    MeasuresPool<int, double> mp;
    for (int i=0; i<n_elem; i++) {
        mp.push(0, xs[i]);
    }

    Estimator_PWM<int, double> mle;
    mle.set_source_evt_approach(&typeid(EVTApproach_BM<int, double>));
    mle.run(mp);
    auto res = mle.get_result();

    ASSERT_TRUE(instanceof_ptr<GEV_Distribution>(res));

    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_location(), 1, 5e-1);    
    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_scale(), 0.5, 5e-1);
    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_shape(), 0.2, 5e-1);
    EXPECT_GT(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_shape(), 0);

}

TEST_F(EstimatorPWM_Test, GEV_13_04_M01) {

    constexpr int n_elem = 1584;
    double xs[n_elem] =
    {

14.3157,14.7115,13.5772,13.9443,13.6284,14.2641,13.3719,13.7431,13.6466,12.8505,14.5476,13.7494,14.0811,14.0409,13.3246,13.6003,12.9909,14.9142,13.7036,14.2337,13.6096,13.9346,13.3924,14.5887,14.0099,13.4948,14.5074,14.4202,13.2052,14.2542,13.3772,13.845,14.3238,13.2973,13.6576,13.685,14.1736,13.4478,13.3348,13.4758,13.6007,13.4686,13.4687,13.5404,14.3922,13.8336,13.2415,13.8006,14.1372,14.2832,14.4718,13.9267,15.3801,14.1272,13.3658,14.2372,13.3336,14.0685,14.2734,13.1437,15.3264,13.9656,13.3978,13.5487,13.8487,13.2115,13.1547,14.4971,15.0538,13.0458,13.6831,13.8851,14.0204,14.0465,13.3804,13.6529,14.6315,13.6448,13.6514,13.57,13.4159,14.034,14.5292,14.4334,14.1491,12.9367,14.2723,13.9732,13.9259,13.9903,13.8598,13.4623,13.1696,14.462,13.4225,13.5201,13.6283,14.7928,13.8623,13.8362,14.4486,14.5612,13.8671,13.6059,13.1533,13.8104,13.449,13.9009,13.6191,14.7104,13.5089,14.633,13.9567,12.9809,13.8841,13.9661,14.0404,13.8558,14.1653,14.0805,13.0177,13.9637,13.6223,14.5205,13.4408,13.6498,13.3914,14.1551,14.2212,14.7989,13.6555,13.6339,13.6061,13.7162,13.3709,14.2202,14.0378,14.0442,14.683,14.251,13.8734,14.0366,13.9251,13.8738,14.0178,14.2245,13.3549,14.2154,13.7562,14.2376,14.8534,14.3747,14.0288,13.5162,14.6437,13.745,13.5362,14.7588,13.9807,12.9128,13.8457,12.8447,14.7314,14.4861,14.195,13.1476,13.8094,13.4056,13.4343,14.4087,13.6083,13.6034,14.1282,13.8964,13.8289,13.6677,13.557,13.9601,13.5692,13.5889,13.8516,13.7039,14.8999,13.9257,13.6208,13.9406,13.1793,13.4961,14.5258,13.4843,13.1063,14.3722,14.5931,13.9899,13.8358,13.2483,13.1372,14.0754,13.8069,13.8454,13.7786,13.8941,13.9361,14.2936,13.6835,13.5384,14.2114,14.0617,13.9569,14.196,13.7357,13.4141,13.1462,13.2216,14.3627,13.6071,13.4017,13.9514,13.5451,13.8822,13.9964,14.1814,15.468,14.4441,14.6575,13.8242,14.2711,13.6158,13.5978,13.911,14.6271,13.0794,14.3617,13.3049,14.3512,14.3648,14.2883,13.1848,13.5788,13.3315,14.8267,14.3308,14.29,12.712,13.6657,13.4073,14.362,15.1401,14.5781,14.597,13.5889,13.8422,14.7665,14.0845,13.9551,14.5291,12.9625,13.3967,13.6192,12.9983,13.5836,13.929,14.3005,14.2213,13.0767,14.1888,14.2324,13.4999,13.7087,13.7994,14.5134,13.6571,13.7518,13.8095,13.3509,13.5609,13.958,14.1531,14.2035,13.4069,14.9998,14.1699,13.9222,13.8464,14.0422,13.958,13.9513,14.8436,13.1083,13.4887,13.1455,13.5161,14.9059,13.5896,13.7337,14.7652,14.3892,13.8185,13.2508,13.1592,13.7826,13.7323,13.8738,13.896,14.311,12.8206,13.3333,14.352,14.2931,13.6544,13.0349,14.5895,14.0918,13.1691,13.7828,13.4177,13.4183,13.8215,13.464,13.6953,14.3828,14.043,13.9905,13.5486,13.3233,13.4897,13.4636,13.6151,13.4309,14.0574,13.6877,12.7279,14.234,13.7388,15.3589,14.5107,14.0557,13.5049,15.0352,13.566,14.1583,13.2914,13.3363,13.5459,14.3133,13.6815,13.9855,13.7086,13.7109,13.9274,13.9666,14.396,13.4933,13.982,13.9416,13.5746,13.215,14.3238,14.0938,13.9826,14.7874,14.0576,13.5872,13.5668,13.3682,14.5607,13.4969,13.903,13.8211,13.6657,14.4449,14.3444,13.535,14.5077,13.555,14.194,13.3484,14.9524,12.7418,13.4166,13.906,14.1417,13.2818,13.8657,14.0675,13.1577,14.1686,13.2743,14.1852,14.5814,13.8821,13.532,13.3149,13.8357,14.7683,13.4943,13.37,14.0652,13.7302,13.2845,13.6243,13.5664,13.9211,14.5706,13.2129,15.3158,14.0659,13.3526,14.6379,13.295,13.946,13.6146,13.7857,13.3779,13.4603,13.7441,13.3784,13.6641,12.9784,13.3155,13.0423,13.6495,14.4948,14.0207,14.091,13.6839,14.223,13.6858,13.9402,13.12,13.3385,13.6938,14.1873,13.7522,13.0554,14.3369,13.3941,13.1397,13.9163,13.3249,14.3159,13.4074,14.2647,13.2016,13.4066,13.3865,13.8057,13.4267,13.4663,13.5717,14.0436,13.9637,14.4359,13.749,13.8335,13.3003,13.8557,13.7599,13.3154,13.5563,14.4646,13.8976,14.4705,13.7521,14.7622,13.6014,13.6171,13.2785,13.4428,14.9338,13.1938,13.1624,13.1393,14.3831,12.8322,13.7191,14.1492,13.4058,14.1837,13.2767,14.7346,14.3222,13.6575,13.0345,13.3409,14.1644,14.0205,13.1148,13.5524,14.077,14.5926,13.6738,13.4091,13.9836,14.5245,12.6849,13.8624,14.1938,14.5273,13.2168,13.9984,13.6752,13.6448,13.8659,13.5833,13.6273,14.451,13.5963,13.7923,15.1697,14.1723,14.7294,13.5244,12.7883,13.2425,13.6289,12.872,12.8292,13.9577,14.0179,14.832,13.592,15.4553,14.1054,13.1833,15.3723,13.5469,14.4827,13.8321,13.6637,13.6038,13.5852,14.2048,14.3229,14.9544,13.7483,14.4841,13.1977,13.6217,13.5555,14.0315,13.5009,15.3172,13.2379,14.2366,13.8364,13.6712,13.4931,13.5772,13.034,13.8971,14.405,13.522,14.1126,13.6467,15.1063,13.7825,13.3227,13.695,13.5926,13.3697,13.6599,14.6818,14.9437,14.6489,13.0049,14.0659,13.6604,12.6439,14.4121,13.111,13.7263,14.8189,14.4434,14.1947,14.9262,13.3053,14.3312,15.9225,13.4133,13.324,13.2588,13.887,13.9854,13.5959,13.5929,13.8943,15.1067,13.625,13.7027,13.4583,13.9219,14.2056,13.6884,13.7543,13.5307,14.808,14.5044,14.7326,13.8897,14.1087,14.2403,14.5209,13.9689,14.2035,13.8159,13.5974,14.5029,14.2842,13.257,12.9249,13.5402,13.7383,13.8447,14.8293,14.1519,14.2872,14.582,14.0028,13.599,13.5322,13.8093,13.5829,13.3535,14.8258,13.689,14.023,14.4279,14.8201,14.9182,14.7112,13.5185,12.9703,14.9913,14.3111,14.2621,13.6203,14.2589,13.6575,13.2313,14.2814,13.571,13.5595,13.7228,14.2668,13.1416,14.2689,13.4492,13.4045,13.4919,13.5769,13.4099,13.5707,14.8467,14.1378,13.3216,13.516,14.0954,13.6085,13.8943,14.4021,13.3517,14.3212,13.9256,13.2203,14.1851,13.8628,13.0368,13.7841,13.4195,14.4538,13.8771,14.2648,14.5223,13.9191,14.3911,13.8173,13.8856,14.0242,14.1222,13.9657,13.5278,13.1699,13.6129,13.4555,13.629,13.622,12.9977,12.861,13.677,13.4687,13.8363,14.747,13.7859,13.8493,14.1548,14.2849,13.624,13.5565,13.5446,13.749,14.1305,13.8032,13.7445,14.0226,14.5662,14.0366,14.083,13.0092,12.9895,13.9243,13.0951,13.8235,13.9058,13.0924,14.3393,13.9166,14.6651,13.2707,13.8143,13.5575,14.6344,13.4785,14.0855,14.42,15.4899,13.7381,13.4352,14.3853,13.4613,13.9861,14.0732,14.6546,13.7028,14.3861,13.5205,13.5856,14.1253,14.1415,14.2616,14.3532,14.2202,13.505,13.6827,15.1964,13.5562,13.5056,14.4758,15.0087,13.8971,13.7415,14.1172,13.5673,14.2817,14.3967,14.078,14.2139,14.273,14.1458,13.4381,13.9456,13.5499,13.3854,14.0866,14.0152,13.7332,13.9308,12.9509,15.3324,14.1249,15.5466,14.3124,13.6625,13.96,14.4168,13.7494,13.6817,13.3817,13.9354,13.0138,13.0981,14.1731,14.6692,14.5938,13.9274,13.5414,13.4196,13.6815,14.2019,13.5943,13.9951,13.5885,13.4532,14.1346,13.722,13.6921,13.9862,13.1595,14.6468,13.9645,13.4833,12.9345,13.7729,14.329,13.7288,14.7008,13.9227,14.3935,12.879,15.1143,14.0362,13.407,14.16,13.7726,14.1422,14.0022,14.3164,13.4965,14.4482,14.0619,14.7359,13.7653,13.5587,14.0765,13.1459,13.8405,13.6049,13.8032,15.0225,13.6944,13.4133,14.1084,13.3598,14.5712,14.1552,13.6354,13.2599,13.3855,14.487,13.855,14.1285,13.5616,13.9174,14.0615,13.7046,13.457,13.8504,13.6037,14.5725,13.9408,13.4135,15.1734,12.9988,13.6818,14.0381,14.0851,13.7135,14.5943,13.7361,13.6087,13.3034,14.1593,13.8368,14.3474,13.4673,13.6696,13.1318,13.383,13.5798,13.6156,13.9819,13.4981,13.622,13.4567,13.3682,12.9793,14.4162,13.3658,14.9983,13.8777,13.9811,13.6084,13.7739,13.7535,14.1344,14.1037,13.6434,12.8421,13.5865,14.4827,14.2951,14.0998,14.3157,13.7436,13.5336,14.3319,15.3546,13.4774,13.0706,13.3363,14.3977,13.674,13.6037,14.1228,14.869,13.8437,14.2953,13.4939,14.2929,13.8745,14.0222,13.8611,13.9161,13.7559,13.9295,13.5119,14.557,13.6527,13.4009,14.9064,13.8118,14.1445,13.8114,13.5295,13.725,13.5704,14.3106,14.6912,13.6874,13.3605,13.7109,14.137,13.2399,13.5447,14.1592,13.965,14.0128,14.318,13.729,14.0858,13.5689,14.0576,13.982,13.4003,13.8275,13.4124,13.5964,14.1128,13.7996,14.2113,13.05,13.4978,14.4909,14.025,14.2486,13.4378,13.3642,13.9611,13.5237,13.6764,13.6782,14.6533,13.6354,14.3579,13.5378,13.8632,13.9851,12.9696,14.8896,13.9191,13.3947,14.0149,13.9174,13.3515,13.2867,13.7833,13.6954,13.9648,15.5562,14.5209,13.5066,13.3229,14.9054,14.7933,13.7562,14.0127,13.7221,13.228,13.198,13.0469,13.8318,13.5231,12.8638,13.8104,13.5523,13.7245,13.1692,15.1306,13.6307,15.3912,13.7494,14.2581,12.8219,14.5475,13.9367,13.7205,13.0612,13.0054,13.7154,13.5118,14.246,13.9291,13.8293,13.4568,13.7693,13.6191,14.1093,13.9676,14.2805,12.9627,13.9755,13.3325,13.8889,15.4081,13.8636,13.1314,14.4328,13.5992,13.5541,13.8333,13.8027,13.7756,13.7076,13.3495,13.4471,14.3181,14.4099,13.3468,13.5866,13.9694,13.9728,13.577,13.6471,13.9252,13.8859,14.6978,13.2197,14.3344,13.8703,14.5442,13.7671,14.3136,13.4037,14.4221,13.5272,13.2708,14.0717,14.3489,13.1336,14.2892,13.3174,14.5234,13.1601,14.4434,13.1392,13.2783,13.8058,13.8109,13.6801,12.7109,13.8257,14.229,14.2605,13.4641,13.8071,13.5629,13.8001,12.7669,14.2212,14.3144,13.9831,13.2182,13.5545,13.219,14.0641,14.9314,14.0507,13.1278,13.6548,13.5146,14.5811,13.5677,14.7554,14.6167,13.3704,13.3589,14.0424,12.5763,13.5268,14.3096,14.4003,13.3244,14.0111,13.6556,13.7581,14.3303,14.2147,13.7603,13.251,14.4927,13.6171,13.2343,14.5369,13.92,14.069,13.6393,13.9907,14.8575,13.2961,14.3309,13.4183,13.6764,13.517,13.5941,13.653,13.9013,14.0555,13.6945,13.5633,13.2478,14.2903,13.7799,13.2943,13.5913,13.6957,13.169,13.9977,13.4542,13.3647,14.0236,13.588,14.4979,13.8007,13.4326,13.7567,13.1245,13.8318,13.6296,14.9685,13.5974,14.4142,13.39,13.648,13.681,14.1574,14.0887,13.4521,13.4514,13.7126,13.3279,13.4203,13.0748,14.9464,13.6893,13.7067,13.418,13.9232,13.5133,14.1502,13.292,13.4773,13.3262,14.0924,13.9462,14.3818,14.1013,14.531,13.1054,14.639,14.2482,13.7798,13.9826,14.011,13.3925,13.1065,14.3983,14.5057,15.0773,13.6971,13.8137,13.4512,13.7714,13.8393,13.5913,13.9422,13.8715,13.4067,14.8059,14.0371,14.3982,13.5877,13.2667,14.1116,14.5748,13.0108,14.4685,13.6548,13.4953,13.8498,13.9735,13.471,14.4216,13.5573,13.4373,13.1668,15.0025,13.4347,13.1828,14.4666,14.0074,13.3194,13.6119,13.3139,13.4726,13.41,14.7404,15.0203,14.7711,13.9492,15.0034,13.0799,13.2592,13.8825,13.8467,13.4002,14.3283,14.2989,13.8382,13.542,15.0173,13.1397,13.3728,13.8489,14.0247,13.6582,13.7354,13.6755,13.5324,12.794,13.2446,13.9555,13.8216,13.7131,14.0666,14.1643,13.1426,14.1841,13.7735,14.772,13.4532,14.0007,12.5107,13.06,14.0654,14.2555,13.553,15.1649,14.1389,13.2753,13.8749,13.5881,14.8077,13.3399,14.323,13.5781,15.2453,13.8319,13.5809,14.0298,13.5466,14.5263,13.6578,14.6101,13.2624,13.1094,13.5642,13.7174,13.7122,13.3921,14.4924,14.1344,13.326,14.1332,13.9541,13.4273,13.785,13.0839,14.8006,13.5331,13.76,13.7216,13.9271,14.84,13.711,14.1786,13.3043,14.4644,13.0665,14.818,13.5153,13.5753,13.2709,14.3291,13.9913,13.4887,14.8945,13.7391,13.8247,13.196,14.2107,13.9976,14.0144,13.7903,14.0486,14.0078,13.4011,16.0447,13.9704,12.6737,13.7085,13.4819,13.9126,15.0277,13.8069,13.0747,13.9496,13.9258,14.0004,14.1201,13.1188,13.4269,14.9296,13.4484,13.9402,14.0339,14.0156,13.5252,14.3347,14.1508,14.0084,14.5586,13.6941,13.5716,13.6122,13.0904,13.6632,13.9912,14.2509,13.6757,14.1454,13.5599,13.9692,14.0784,13.5613,13.7422,13.8508,14.0634,14.1733,14.0847,13.6904,13.8766,14.0138,14.2042,13.5535,13.1801,13.633,13.4926,12.9584,13.9702,13.4408,13.5341,13.6868,13.2381,14.3592,14.0203,13.5876,13.7841,13.359,14.0539,13.5739,14.6215,14.5724,15.6163,13.4261,13.4523,14.4351,14.1939,13.7137,15.0969,14.292,12.8352,13.5321,14.0145,14.2182,14.7058,13.6358,13.4256,13.4229,13.8569,13.8871,14.1331,12.7249,13.174,14.6582,14.0244,13.3331,13.8167,14.1329,13.6689,13.2821,14.1214,13.0419,14.177,14.3878,15.7479,14.3181,13.2554,13.6306,13.7276,13.1785,14.6813,13.0461,13.2616,13.6643,13.5301,13.7954,13.8466,13.5966,13.1396,13.3679,13.9939,13.62,13.3586,13.4136,14.4014,13.5922,13.5491,13.6889,13.8358,13.7529,13.426,13.5819,14.1346,13.5354,15.0875,13.7665,13.1906,13.7877,14.8557,14.2105,14.1442,13.672,13.4941,13.5936,14.2073,13.6087,13.6203,13.5467,14.13,13.8742,14.0913,14.2247,13.5101,13.7438,14.4508,13.4727,14.0571,14.5548,13.8612,15.1941,13.8411,14.2165,13.2807,13.403,13.812,13.263,14.0858,12.9789,13.903,14.6642,13.4705,14.2886,13.4281,13.8176,13.6245,15.0777,13.4476,12.9158,13.3409,13.79,13.8096,14.0479,13.4757,13.6152,14.0813,13.3672,13.8858,14.537,13.1083,14.1542,13.2452,13.2998,14.2674,12.8534,13.878,14.4178,14.3333,12.5428,13.7623,14.5852,12.9427,14.5451,14.1954,13.2891,13.739,13.4765,13.6885,14.4447,12.9035,13.962,14.2151,13.8336,13.3474,14.7062,15.4943,13.6771,13.1079,13.8363,13.3572,12.9726,13.7572,13.9429,15.0736,13.5734,14.2321,14.7508,13.7093,13.6325,13.6298,13.8955,14.4985,13.7202,14.4719,13.9388,13.9707,13.9711,13.5474,14.4061,13.104,13.7033,13.4763,13.473,14.1192,13.5378,13.3617,13.9549,14.4216,13.2674,14.6918,13.9059,14.104,13.9729,13.8738,13.3819,13.297,14.153,14.1533,14.1884,13.274,14.5362,13.7533,14.3783,13.7049,14.0152

    };
    
    MeasuresPool<int, double> mp;
    for (int i=0; i<n_elem; i++) {
        mp.push(0, xs[i]);
    }

    Estimator_PWM<int, double> mle;
    mle.set_source_evt_approach(&typeid(EVTApproach_BM<int, double>));
    mle.run(mp);
    auto res = mle.get_result();

    ASSERT_TRUE(instanceof_ptr<GEV_Distribution>(res));

    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_location(), 13.641, 1e-3);
    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_scale(), 0.462, 1e-3);
    EXPECT_NEAR(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_shape(), -0.116, 1e-3);
    EXPECT_LT(std::dynamic_pointer_cast<GEV_Distribution>(res)->get_shape(), 0);

}

TEST_F(EstimatorPWM_Test, Exceptions) {

    MeasuresPool<int, double> mp;

    Estimator_PWM<int, double> mle;

    EXPECT_THROW(mle.get_result(), std::runtime_error);

    EXPECT_THROW(mle.run(mp), std::runtime_error);

    mle.set_source_evt_approach(&typeid(int));

    EXPECT_THROW(mle.run(mp), std::runtime_error);

}


TEST_F(EstimatorPWM_Test, String) {
    Estimator<int, double> *test = new Estimator_PWM<int, double>();

    EXPECT_EQ("Probabilistic Weighted Moments (PWM)", test->to_string());

    delete test;
}

