#include "browserhelpers.h"

BrowserHelpers::BrowserHelpers(QObject *parent) : QObject(parent)
{

}

template<typename Arg, typename R>
InvokeWrapper<Arg, R> BrowserHelpers::Invoke(R* receiver, void (R::*memberFunc)(Arg))
{
	InvokeWrapper<Arg, R> wrapper = { receiver, memberFunc };
	return wrapper;
}
