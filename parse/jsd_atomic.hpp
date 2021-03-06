#pragma once

#include "jsd_core.hpp"
#include <atomic>

namespace JSON
{
	template <typename T>
	void parse(std::atomic<T>& value, std::string const& name,
               PropertyTree const& object, ParsingOptions const& options = {})
	{
		try
		{
			T temp;
			parse(temp, name, object, options);
			value.store(std::move(temp));
		}
		catch (boost::property_tree::ptree_bad_data& exc)
		{
			DEFAULT_PROPERTY_ERROR_HANDLER(T(), T());
		}
        catch (boost::property_tree::ptree_bad_path& exc)
        {
			DEFAULT_PATH_ERROR_HANDLER(T(), T());
        }
	}
}
