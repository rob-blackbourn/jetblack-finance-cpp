#ifndef __jetblack__options__BlackScholes_hpp
#define __jetblack__options__BlackScholes_hpp

#include <cmath>
#include "maths/distributions.hpp"
#include "NR/RootFinder.hpp"

namespace options
{
	enum OptionType
	{	
		Call,
		Put	
	};

	enum CashPhysical
	{	
		Cash,
		Phys	
	};

	struct GBlackScholesOutputs
	{
		double premium;
		double delta;
		double gamma;
		double vega;
		double theta;
		double rho;
		double theta_mart;
		double volga;
		double vanna;	
		double speed;
		double charm;
	};

	inline double BSprice(OptionType optType, double spotPrice, double strikePrice, double timeToExpiry, double rate, double dividendYield, double volatility)
	{	
		double d1 = (::log(spotPrice / strikePrice) + ((rate - dividendYield) + (volatility * volatility) / 2.0) * timeToExpiry) / (volatility * ::sqrt(timeToExpiry));
		double d2 = d1 - volatility * ::sqrt(timeToExpiry);
		
		switch (optType)
		{
			case Call:
				return ( spotPrice * ::exp(-dividendYield * timeToExpiry) * maths::stdnormal_cdf(d1) ) - ( strikePrice * ::exp(-rate * timeToExpiry) * maths::stdnormal_cdf(d2) );
			case Put:
				return ( strikePrice * ::exp(-rate * timeToExpiry) * maths::stdnormal_cdf(-d2) ) - ( spotPrice * ::exp(-dividendYield* timeToExpiry) * maths::stdnormal_cdf(-d1) );
			default:
				throw "Invalid Option Type";
		}
	}

	inline double BSdelta(OptionType optType, double spotPrice, double strikePrice, double timeToExpiry, double rate, double dividendYield, double volatility)
	{
		double d1 = (::log(spotPrice / strikePrice) + ((rate - dividendYield) + (volatility * volatility) / 2.0) * timeToExpiry) / (volatility * ::sqrt(timeToExpiry));
		
		switch (optType)
		{
			case Call:
				return ::exp(-dividendYield * timeToExpiry) * maths::stdnormal_cdf(d1);
			case Put:
				return ::exp(-dividendYield * timeToExpiry) * (maths::stdnormal_cdf(d1) - 1.0);
			default:
				throw "Invalid Option Type";
		}
	}

	inline double BSgamma(OptionType optType, double spotPrice, double strikePrice, double timeToExpiry, double rate, double dividendYield, double volatility)
	{
		double d1 = (::log(spotPrice / strikePrice) + ((rate - dividendYield) + (volatility * volatility) / 2.0) * timeToExpiry) / (volatility * ::sqrt(timeToExpiry));
		
		switch (optType)
		{
			case Call:
				return ::exp(-dividendYield * timeToExpiry) * maths::norm_pdf(d1)/(spotPrice*volatility*sqrt(timeToExpiry));		
			case Put:
				return ::exp(-dividendYield * timeToExpiry) * maths::norm_pdf(d1)/(spotPrice*volatility*sqrt(timeToExpiry));
			default:
				throw "Invalid Option Type";
		}
	}

	inline double BSpriceCQS(double spotPrice, double PVDivs, double strikePrice, double volatility, double T, double stockBorrow, double borrowTime, double discSettleV, double discSettleE, double discSpotE, OptionType optType, CashPhysical cashPhys)
	{
		double CQSrate = - log(discSettleE / discSettleV) / T;
		double SpotAdj = spotPrice - PVDivs / discSettleV;
		double CQSborrow = stockBorrow * borrowTime;

		switch(cashPhys)
		{
		case Phys:
			return BSprice(optType, SpotAdj, strikePrice, T, CQSrate, CQSborrow, volatility) * discSettleV;
		case Cash:
			return BSprice(optType, SpotAdj, strikePrice, T, CQSrate, CQSborrow, volatility) * discSettleV * discSpotE/discSettleE;	
		default:
			throw "Invalid Settlement Type";
		}
	}

	inline double BSdeltaCQS(double spotPrice, double PVDivs, double strikePrice, double volatility, double T, double stockBorrow, double borrowTime, double discSettleV, double discSettleE, double discSpotE, OptionType optType, CashPhysical cashPhys)
	{
		double CQSrate = - log(discSettleE / discSettleV) / T;
		double SpotAdj = spotPrice - PVDivs / discSettleV;
		double bumpedSpotAdj = spotPrice * 1.001 - PVDivs / discSettleV;
		double CQSborrow = stockBorrow * borrowTime;
		double bumpedprice;
		double rawprice;

		switch(cashPhys)
		{
		case Phys:
			rawprice = BSprice(optType, SpotAdj, strikePrice, T, CQSrate, CQSborrow, volatility) * discSettleV;
			bumpedprice = BSprice(optType, bumpedSpotAdj, strikePrice, T, CQSrate, CQSborrow, volatility) * discSettleV;
			return (bumpedprice-rawprice) / (spotPrice*0.001);
		case Cash:
			rawprice = BSprice(optType, SpotAdj, strikePrice, T, CQSrate, CQSborrow, volatility) * discSettleV * discSpotE/discSettleE;
			bumpedprice = BSprice(optType, bumpedSpotAdj, strikePrice, T, CQSrate, CQSborrow, volatility) * discSettleV * discSpotE/discSettleE;
			return (bumpedprice-rawprice) / (spotPrice*0.001);
		default:
			throw "Invalid Settlement Type";
		}
	}

	struct ivol_of : NR::RootFinder::RootOF
	{
		virtual double operator()(double volatility)
		{
			double modelPrice = options::BSprice(optType, spotPrice, strikePrice, timeToExpiry, rate, dividendYield, volatility);
			return modelPrice - marketPrice;
		}

		options::OptionType optType;
		double spotPrice;
		double strikePrice;
		double timeToExpiry;
		double rate;
		double dividendYield;
		double marketPrice;
	};

	inline double BSivol(OptionType optType, double spotPrice, double strikePrice, double timeToExpiry, double rate, double dividendYield, double marketPrice)
	{
		ivol_of of;
		of.optType = optType;
		of.spotPrice = spotPrice;
		of.strikePrice = strikePrice;
		of.timeToExpiry = timeToExpiry;
		of.rate = rate;
		of.dividendYield = dividendYield;
		of.marketPrice = marketPrice;

		NR::RootFinder rf;
		double lower_bound = 0.2, upper_bound = 0.3;
		if (!rf.findBracket(of, lower_bound, upper_bound))
			throw std::runtime_error("unable to find bracket");

		return rf.findRoot(of, lower_bound, upper_bound);
	}

	inline GBlackScholesOutputs GBlackScholes(OptionType optType, double spotPrice, double pvDiv, double strikePrice, double timeToExpiry, double rate, double dividendYield, double volatility)
	{
		GBlackScholesOutputs outputs;		
		double spotMinPVDiv = spotPrice - pvDiv; 
		double d1 = (::log(spotMinPVDiv / strikePrice) + ((rate - dividendYield) + (volatility * volatility) / 2.0) * timeToExpiry) / (volatility * ::sqrt(timeToExpiry));
		double d2 = d1 - volatility * ::sqrt(timeToExpiry);
		double h = 0.0001;
		double hs = 0.0001;
		double hv = 0.0001;
		
		double tstSpeed = 0;

		switch(optType)
		{
			case Call:
				if (timeToExpiry <= 0)
				{
					outputs.premium = 0;
					outputs.delta = 0;
					outputs.gamma = 0;
					outputs.vega = 0;
					outputs.theta = 0;
					outputs.rho = 0;
					outputs.theta_mart = 0;
					outputs.charm = 0;
					outputs.volga = 0;
					outputs.vanna = 0;
					outputs.speed = 0;
				}
				else
				{
					outputs.premium = ( spotMinPVDiv * ::exp(-dividendYield * timeToExpiry) * maths::stdnormal_cdf(d1) ) - ( strikePrice * ::exp(-rate * timeToExpiry) * maths::stdnormal_cdf(d2) );
					outputs.delta = ::exp(-dividendYield * timeToExpiry) * maths::stdnormal_cdf(d1) ;			
					outputs.gamma = (BSdelta(optType,(spotPrice * 1.00001 - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility) - BSdelta(optType,(spotPrice - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility)) / (spotPrice * 1.00001 - spotPrice);
					//outputs.vega = ( ( BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, (volatility + 0.00001)) ) - outputs.premium) / 0.00001;
					outputs.vega = ( BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility + 2.0 * h) - BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility - 2.0 * h) ) / (4.0 * h);

					if(timeToExpiry<=(1.0/365.0)) 
					{
						outputs.theta = BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, 0.0) - outputs.premium;
						outputs.theta_mart = ::exp(-rate*timeToExpiry) * BSprice(optType, spotMinPVDiv * ::exp((rate-dividendYield)*timeToExpiry), strikePrice, timeToExpiry, 0.0, 0.0, 0.0)  - outputs.premium;
						outputs.charm = (BSdelta(optType,spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, 0.0) - BSdelta(optType,spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility));
					}
					else 
					{
						outputs.theta = ( ( BSprice(optType, spotMinPVDiv, strikePrice, (timeToExpiry - 1.0/365.0), rate, dividendYield, volatility) ) - outputs.premium);								
						outputs.theta_mart = ::exp(-rate*timeToExpiry) * BSprice(optType, spotMinPVDiv * ::exp((rate-dividendYield)*timeToExpiry), strikePrice, timeToExpiry - 1.0/365.0, 0.0, 0.0, volatility)  - outputs.premium;
						outputs.charm = (BSdelta(optType,spotMinPVDiv, strikePrice, (timeToExpiry - 1.0/365.0), rate, dividendYield, volatility) - BSdelta(optType,spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility));
					}
				
					outputs.rho = ( (BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate - 0.0001, dividendYield, volatility) ) - outputs.premium);								
					outputs.volga = ( BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility - 2.0 * h) + 4.0 * BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility - h) - 10.0 * BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility ) + 4.0 * BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility + h) + BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility + 2.0 * h) ) / (8.0 * h * h);
					outputs.vanna = ( BSprice(optType, spotMinPVDiv + hs, strikePrice, timeToExpiry, rate, dividendYield, volatility + hv) - BSprice(optType, spotMinPVDiv - hs, strikePrice, timeToExpiry, rate, dividendYield, volatility + hv) - BSprice(optType, spotMinPVDiv + hs, strikePrice, timeToExpiry, rate, dividendYield, volatility - hv) + BSprice(optType, spotMinPVDiv - hs, strikePrice, timeToExpiry, rate, dividendYield, volatility - hv)) / (4.0 * hs * hv);			
					outputs.speed = (BSgamma(optType,(spotPrice * 1.00001 - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility) - BSgamma(optType,(spotPrice - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility)) / (spotPrice * 1.00001 - spotPrice);
					tstSpeed = -::exp(dividendYield * timeToExpiry) * maths::norm_pdf(d1)/(spotMinPVDiv*spotMinPVDiv*volatility*::sqrt(timeToExpiry))*(d1/(volatility * ::sqrt(timeToExpiry))+1);
				}
				break;
			case Put:
				if (timeToExpiry <= 0)
				{
					outputs.premium = 0;
					outputs.delta = 0;
					outputs.gamma = 0;
					outputs.vega = 0;
					outputs.theta = 0;
					outputs.charm = 0;
					outputs.rho = 0;
					outputs.theta_mart = 0;
					outputs.volga = 0;
					outputs.vanna = 0;
					outputs.speed = 0;
				}

				else
				{
					outputs.premium = ( strikePrice * ::exp(-rate * timeToExpiry) * maths::stdnormal_cdf(-d2) ) - ( spotMinPVDiv * ::exp(-dividendYield* timeToExpiry) * maths::stdnormal_cdf(-d1) );
					outputs.delta = ::exp(-dividendYield * timeToExpiry) * (maths::stdnormal_cdf(d1) - 1.0);
					outputs.gamma = (BSdelta(optType,(spotPrice * 1.00001 - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility) - BSdelta(optType,(spotPrice - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility)) / (spotPrice * 1.00001 - spotPrice);
					//outputs.vega = ( ( BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, (volatility + 0.00001)) ) - outputs.premium) / 0.00001;
					outputs.vega = ( BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility + 2.0 * h) - BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility - 2.0 * h) ) / (4.0 * h);

					if(timeToExpiry<=(1.0/365.0))
					{
						outputs.theta =  BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, 0.0) - outputs.premium;
						outputs.theta_mart = ::exp(-rate*timeToExpiry) * BSprice(optType, spotMinPVDiv * ::exp((rate-dividendYield)*timeToExpiry), strikePrice, timeToExpiry, 0.0, 0.0, 0.0)  - outputs.premium;
						outputs.charm = (BSdelta(optType,spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, 0.0) - BSdelta(optType,spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility));
					}
					else
					{
						outputs.theta = ( ( BSprice(optType, spotMinPVDiv, strikePrice, (timeToExpiry - 1.0/365.0), rate, dividendYield, volatility) ) - outputs.premium) ;
						outputs.theta_mart = ::exp(-rate*timeToExpiry) * BSprice(optType, spotMinPVDiv * ::exp((rate-dividendYield)*timeToExpiry), strikePrice, timeToExpiry - 1.0/365.0, 0.0, 0.0, volatility)  - outputs.premium;
						outputs.charm = (BSdelta(optType,spotMinPVDiv, strikePrice, (timeToExpiry - 1.0/365.0), rate, dividendYield, volatility) - BSdelta(optType,spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility));
					}
					outputs.rho = ( (BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate - 0.0001, dividendYield, volatility) ) - outputs.premium);		
					outputs.volga = ( BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility - 2.0 * h) + 4.0 * BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility - h) - 10.0 * BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility ) + 4.0 * BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility + h) + BSprice(optType, spotMinPVDiv, strikePrice, timeToExpiry, rate, dividendYield, volatility + 2.0 * h) ) / (8.0 * h * h);
					outputs.vanna = ( BSprice(optType, spotMinPVDiv + hs, strikePrice, timeToExpiry, rate, dividendYield, volatility + hv) - BSprice(optType, spotMinPVDiv - hs, strikePrice, timeToExpiry, rate, dividendYield, volatility + hv) - BSprice(optType, spotMinPVDiv + hs, strikePrice, timeToExpiry, rate, dividendYield, volatility - hv) + BSprice(optType, spotMinPVDiv - hs, strikePrice, timeToExpiry, rate, dividendYield, volatility - hv)) / (4.0 * hs * hv);
					outputs.speed = (BSgamma(optType,(spotPrice * 1.00001 - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility) - BSgamma(optType,(spotPrice - pvDiv), strikePrice, timeToExpiry, rate, dividendYield, volatility)) / (spotPrice * 1.00001 - spotPrice);
				}
				break;
			default:
				throw "Invalid option type";
		}

		return outputs;
	}
}

#endif // __jetblack__options__BlackScholes_hpp
