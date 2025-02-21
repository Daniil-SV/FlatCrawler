#pragma once

#include <unordered_map>

namespace flatCrawler
{
	template<typename T>
	class GuessMachine
	{
	public:
		void guess(const T value, int priority = 1)
		{
			if (!m_map.count(value))
			{
				m_map[value] = priority;
			}
			else
			{
				m_map[value] += priority;
			}

			m_total_guess++;
		}

		T most_probable(const T default_value) const
		{
			if (m_map.empty()) return default_value;

			auto it = std::max_element(
				m_map.begin(), m_map.end(), [](auto& a, auto& b)
				{
					return a.second > b.second;
				}
			);

			return it->first;
		}

		// Returns a positive value if field type is ambiguous or cannot be guessed with certainty
		bool is_floating() const { return m_map.size() != 1; }

	private:
		std::unordered_map<T, int> m_map;
		size_t m_total_guess = 0;
	};
}