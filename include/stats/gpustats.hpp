#ifndef STATS_GPUSTATS_HPP
#define STATS_GPUSTATS_HPP

#include "provider.hpp"

namespace am {
	class GPUStats final : public StatsProvider {
	private:
	public:
		GPUStats();

		void start() override;
		void stop() override;
		void getStats(Stats& stats) override;
	};
} // namespace am

#endif