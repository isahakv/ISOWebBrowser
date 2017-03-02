#ifndef BROWSERHELPERS_H
#define BROWSERHELPERS_H

//#include <QObject>

namespace BrowserHelpers
{
	template<typename Arg, typename R>
	struct InvokeWrapper
	{
		R* receiver;
		void (R::*memberFunc)(Arg);

		void operator()(Arg result)
		{
			(receiver->*memberFunc)(result);
		}
	};

	template<typename Arg, typename R>
	InvokeWrapper<Arg, R> Invoke(R* receiver, void (R::*memberFunc)(Arg))
	{
		InvokeWrapper<Arg, R> wrapper = { receiver, memberFunc };
		return wrapper;
	}
}

#endif // BROWSERHELPERS_H
