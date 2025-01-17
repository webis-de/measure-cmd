#ifndef STATFORMATTER_HPP
#define STATFORMATTER_HPP

#include "stats/provider.hpp"

#include <ostream>

class StatFormatter {
public:
	virtual void formatInto(std::ostream& stream) const noexcept = 0;

	friend std::ostream& operator<<(std::ostream& stream, const StatFormatter& formatter) {
		formatter.formatInto(stream);
		return stream;
	}
};

class SimpleFormatter final : public StatFormatter {
private:
	const am::Stats stats;

public:
	explicit SimpleFormatter(const am::Stats& stats);

	virtual void formatInto(std::ostream& stream) const noexcept override;
};

class JsonFormatter final : public StatFormatter {
private:
	const am::Stats stats;

public:
	explicit JsonFormatter(const am::Stats& stats);

	virtual void formatInto(std::ostream& stream) const noexcept override;
};

#endif