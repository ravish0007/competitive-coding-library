//refference: https://discuss.codechef.com/questions/65993/rng-editorial

const int mod = 1e9+7;
 
inline void add(int &a, int b) {
    a += b;
    if (a >= mod) a -= mod;
}
inline void sub(int &a, int b) {
    a -= b;
    if (a < 0) a += mod;
}

inline int mul(int a, int b) {
#if !defined(_WIN32) || defined(_WIN64)
    return (int) ((long long) a * b % mod);
#endif
    unsigned long long x = (long long) a * b;
    unsigned xh = (unsigned) (x >> 32), xl = (unsigned) x, d, m;
    asm(
        "divl %4; \n\t"
        : "=a" (d), "=d" (m)
        : "d" (xh), "a" (xl), "r" (mod)
    );
    return m;
}


inline int power(int a, long long b) {
    int res = 1;
    while (b > 0) {
        if (b & 1) {
            res = mul(res, a);
        }
        a = mul(a, a);
        b >>= 1;
    }
    return res;
}
inline int inv(int a) {
    a %= mod;
    if (a < 0) a += mod;
    int b = mod, u = 0, v = 1;
    while (a) {
        int t = b / a;
        b -= t * a; swap(a, b);
        u -= t * v; swap(u, v);
    }
    assert(b == 1);
    if (u < 0) u += mod;
    return u;
}

//attribution: Jatin Yadav on codechef
namespace fft{
    #define ld double
    #define poly vector<ll>

    struct base{
        ld x,y;
        base(){x=y=0;}
        base(ld _x, ld _y){x = _x,y = _y;}
        base(ld _x){x = _x, y = 0;}
        void operator = (ld _x){x = _x,y = 0;}
        ld real(){return x;}
        ld imag(){return y;}
        base operator + (const base& b){return base(x+b.x,y+b.y);}
        void operator += (const base& b){x+=b.x,y+=b.y;}
        base operator * (const base& b){return base(x*b.x - y*b.y,x*b.y+y*b.x);}
        void operator *= (const base& b){ld p = x*b.x - y*b.y, q = x*b.y+y*b.x; x = p, y = q;}
        void operator /= (ld k){x/=k,y/=k;}
        base operator - (const base& b){return base(x - b.x,y - b.y);}
        void operator -= (const base& b){x -= b.x, y -= b.y;}
        base conj(){ return base(x, -y);}
        base operator / (ld k) { return base(x / k, y / k);}
        void Print(){ cerr << x <<  " + " << y << "i\n";}
    };
    double PI = 2.0*acos(0.0);
    const int MAXN = 17;
    const int maxn = 1<<MAXN;
    base W[maxn],invW[maxn], P1[maxn], Q1[maxn];
    bool fst = 1;
    void precompute_powers(){
        for(int i = 0;i<maxn/2;i++){
            double ang = (2*PI*i)/maxn; 
            ld _cos = cos(ang), _sin = sin(ang);
            W[i] = base(_cos,_sin);
            invW[i] = base(_cos,-_sin);
        }
    }
    void fft (vector<base> & a, bool invert) {
    	if(fst) precompute_powers(), fst = 0;
        int n = (int) a.size();
     
        for (int i=1, j=0; i<n; ++i) {
            int bit = n >> 1;
            for (; j>=bit; bit>>=1)
                j -= bit;
            j += bit;
            if (i < j)
                swap (a[i], a[j]);
        }
        for (int len=2; len<=n; len<<=1) {
            for (int i=0; i<n; i+=len) {
                int ind = 0,add = maxn/len;
                for (int j=0; j<len/2; ++j) {
                    base u = a[i+j],  v = (a[i+j+len/2] * (invert?invW[ind]:W[ind]));
                    a[i+j] = (u + v);
                    a[i+j+len/2] = (u - v);
                    ind += add;
                }
            }
        }
        if (invert) for (int i=0; i<n; ++i) a[i] /= n;
    }

    // 4 FFTs in total for a precise convolution
    poly mult(poly &a, poly &b, ll mod){
        int n1 = a.size(),n2 = b.size();
        int final_size = a.size() + b.size() - 1;
        int n = 1;
        while(n < final_size) n <<= 1;
        vector<base> P(n), Q(n);
        int SQRTMOD = (int)sqrt(mod) + 10;
        for(int i = 0;i < n1;i++) P[i] = base(a[i] % SQRTMOD, a[i] / SQRTMOD);
        for(int i = 0;i < n2;i++) Q[i] = base(b[i] % SQRTMOD, b[i] / SQRTMOD);
        fft(P, 0);
        fft(Q, 0);
        base A1, A2, B1, B2, X, Y;
        for(int i = 0; i < n; i++){
            X = P[i];
            Y = P[(n - i) % n].conj();
            A1 = (X + Y) * base(0.5, 0);
            A2 = (X - Y) * base(0, -0.5);
            X = Q[i];
            Y = Q[(n - i) % n].conj();
            B1 = (X + Y) * base(0.5, 0);
            B2 = (X - Y) * base(0, -0.5);
            P1[i] = A1 * B1 + A2 * B2 * base(0, 1);
            Q1[i] = A1 * B2 + A2 * B1;
        }
        for(int i = 0; i < n; i++) P[i] = P1[i], Q[i] = Q1[i];
        fft(P, 1);
        fft(Q, 1);
        poly ret(final_size);
        for(int i = 0; i < final_size; i++){
            ll x = (ll)(P[i].real() + 0.5);
            ll y = (ll)(P[i].imag() + 0.5) % mod;
            ll z = (ll)(Q[i].real() + 0.5);
            ret[i] = (x + ((y * SQRTMOD + z) % mod) * SQRTMOD) % mod;
        }
        return ret;
    }
}


//use your favourate fast polynomial multiplication algo here
vll mult(vll a,vll b, int upper_limit = 1e9) {
	vll c;
    int sz1 = a.size();
    int sz2 = b.size();
    if(min(sz1, sz2)<=5){
    	c.resize(sz1 + sz2 - 1);
        for (int i = 0;i < sz1;++i)
            for (int j = 0;j < sz2;++j)
                c[i + j]=(c[i+j]+a[i]*b[j])%mod;
    }
    else c=fft::mult(a, b, mod);
    if((int)c.size() > upper_limit) c.resize(upper_limit);
    return c;
}

//attribution: https://www.codechef.com/viewsolution/19110694
namespace poly_ops{
	typedef ll base;
	inline int add(int x, int y){ x += y; if(x >= mod) x -= mod; return x;}
	inline int sub(int x, int y){ x -= y; if(x < 0) x += mod; return x;}

	vector<base> truncate_end(vector<base> v){
	    while(!v.empty() && v.back() == 0) v.pop_back();
	    if(v.empty()) v = {0};
	    return v;
	}

	vector<base> add(vector<base> a, vector<base> b){
	    vector<base> ret(max(a.size(), b.size()));
	    for(int i = 0; i < (int)ret.size(); i++){
	        ret[i] = add(i < (int)a.size() ? a[i] : 0, i < (int)b.size() ? b[i] : 0);
	    }
	    return ret;
	}

	vector<base> sub(vector<base> a, vector<base> b){ 
	    vector<base> ret(max(a.size(), b.size()));
	    for(int i = 0; i < (int)ret.size(); i++){
	        ret[i] = sub(i < (int)a.size() ? a[i] : 0, i < (int)b.size() ? b[i] : 0);
	    }
	    return ret;
	}

	vector<base> mul_scalar(vector<base> v, int k){
	    for(auto & it : v) it = mul(k, it);
	    return v;
	}

	vector<base> get_first(vector<base> v, int k){
	    v.resize(min((int)v.size(), k));
	    return v;
	}

	vector<base> inverse(vector<base> a, int sz){
	    assert(a[0] != 0);
	    vector<base> x = {inv(a[0])};
	    while((int)x.size() < sz){
	        vector<base> temp(a.begin(), a.begin() + min(a.size(), 2 * x.size()));
	        vector<base> nx = mult(mult(x, x), temp);
	        x.resize(2 * x.size());
	        for(int i = 0; i < (int)x.size(); i++)
	            x[i] = sub(add(x[i], x[i]), nx[i]);
	    }
	    x.resize(sz);
	    return x;
	}

	vector<base> differentiate(vector<base> f){
	    for(int i = 0; i + 1 < (int)f.size(); i++) f[i] = mul(i + 1, f[i + 1]);
	    if(!f.empty()) f.resize(f.size() - 1);
	    if(f.empty()) f = {0};
	    return f;
	}

	vector<base> integrate(vector<base> f, int c = 0){
	    f.resize(f.size() + 1);
	    for(int i = f.size(); i >= 1; i--) f[i] = mul(f[i - 1], inv(i));
	    f[0] = c;
	    return f;
	}

	vector<base> Log(vector<base> f, int k){
	    assert(f[0] == 1);
	    vector<base> inv_f = inverse(f, k);
	    return integrate(mult(differentiate(f), inv_f, k))   ;
	}

	vector<base> Exp(vector<base> f, int k){
	    assert(f[0] == 0);
	    vector<base> g = {1};
	    while((int)g.size() < k){
	        int curr_sz = g.size();
	        g = mult(g, get_first(add(f, sub({1}, Log(g, 2 * curr_sz))), 2 * curr_sz), 2 * curr_sz);
	    }

	    g.resize(k);
	    return g;
	}

	vector<base> powr(vector<base> X, long long n, int k){
	    int common = X[0];
	    int inv_com = inv(common);
	    X = mul_scalar(X, inv_com);
	    n %= mod;
	    vector<base> ret = Exp(mul_scalar(Log(X, k + 1), n), k);
	    ret.resize(k);
	    ret = mul_scalar(ret, power(common, n));
	    return ret;
	}

	vector<base> divmod(vector<base> f, vector<base> g){
	    if(f.size() < g.size()) return f;
	    int sz = f.size() - g.size() + 1;
	    reverse(f.begin(), f.end()); reverse(g.begin(), g.end());
	    vector<base> inv2 = inverse(g, sz);
	    vector<base> _p = f; _p.resize(sz);
	    vector<base> q = mult(inv2, _p);
	    q.resize(sz);
	    reverse(q.begin(), q.end()); reverse(f.begin(), f.end()); reverse(g.begin(), g.end());
	    return truncate_end(sub(f, mult(g, q)));
	}
}

//recurrance defined by: f[n] = sigma(i from n->1) f[n-i]*rec[n-i]
struct linearRecurrance{
	vll M;	//don't change these
	vll base;	//value of f[0..n-1]
	vll rec;		//value of coefficients of recursion 
	int n;		//size of recursion
	vll pre_powers[61];	//precalculate powers to save time

	linearRecurrance(vll _base, vll _rec):base(_base), rec(_rec), n(_rec.size())
	{
		M.resize(n+1);
		M[n]=1;
		for(int i=0; i<n; ++i)
			M[i]=(mod-rec[i])%mod;
		ll cur;
		for(int i=0; i<61; ++i){
			cur=1LL<<i;
			if(cur<n){
				pre_powers[i].assign(cur+1, 0);
				pre_powers[i][cur]=1;
				continue;
			}
			pre_powers[i]=poly_ops::divmod(mult(pre_powers[i-1], pre_powers[i-1]), M);
		}
	}

	ll kth_term(ll k)
	{
		if(k<(int)base.size()) return base[k];
		vll vec;
		if(k<n){
			vec.assign(k+1, 0);
			vec[k]=1;
		} else {
			vec={1};
			for(int i=0; (1LL<<i)<=k; ++i)
				if((k>>i)&1) vec=poly_ops::divmod(mult(vec, pre_powers[i]), M);
		}
		ll ans=0;
		for(int i=0; i<n; ++i)
			ans=(ans+vec[i]*base[i])%mod;
		return ans;
	}	
};