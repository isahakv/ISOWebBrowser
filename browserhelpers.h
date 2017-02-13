#ifndef BROWSERHELPERS_H
#define BROWSERHELPERS_H

#include <QObject>

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

class BrowserHelpers : public QObject
{
	Q_OBJECT
public:
	explicit BrowserHelpers(QObject *parent = 0);

	template<typename Arg, typename R>
	static InvokeWrapper<Arg, R> Invoke(R* receiver, void (R::*memberFunc)(Arg));
};

#endif // BROWSERHELPERS_H
