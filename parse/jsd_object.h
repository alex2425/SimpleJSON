#ifndef JSD_OBJECT_H_INCLUDED
#define JSD_OBJECT_H_INCLUDED

#include "jsd_core.h"

namespace JSON
{
	namespace Internal
	{
		template <typename Type, typename enable = void>
		class isParsableStruct
		{
		public:
			static const bool value = false;
		};

		template <typename Type>
		class isParsableStruct <Type, typename std::enable_if<std::is_class<Type>::value>::type>
		{
		private:
			class yes { char m; };
			class no { yes m[2];};
			struct BaseMixin
			{
				void parse(std::string const&,
							  PropertyTree const&,
							  ParsingOptions const&)
				{}
			};
			struct Base : public Type, public BaseMixin {};
			template <typename T, T t>  class Helper{};

			template <typename U>
			static no deduce(U*, Helper<
				void (BaseMixin::*)(std::string const&,
									PropertyTree const&,
									ParsingOptions const&),
				&U::parse>* = nullptr);
			static yes deduce(...);
		public:
			static const bool value = sizeof(yes) == sizeof(deduce((Base*)(0)));
		};
	}

	template <typename T>
	typename std::enable_if <Internal::isParsableStruct<T>::value, void>::type
	parse(T& value, std::string const& name, PropertyTree const& tree, ParsingOptions const& options = DEFAULT_PARSER_OPTIONS)
	{
		value.parse(name, tree, options);
	}
}

#endif // JSD_OBJECT_H_INCLUDED