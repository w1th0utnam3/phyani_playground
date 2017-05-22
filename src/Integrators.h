#pragma once

namespace StateIntegrators
{

template <typename SystemFunT, typename StateT, typename DataT>
StateT explicit_euler(SystemFunT fun, double initialTime, double dt, StateT initialState, DataT data)
{
	return initialState + dt*fun(initialTime, initialState, data);
}
	
template <typename SystemFunT, typename StateT, typename DataT>
StateT rk2(SystemFunT fun, double initialTime, double dt, StateT initialState, DataT data)
{
	auto k1 = dt*fun(initialTime, initialState, data);
	auto k2 = dt*fun(initialTime + 0.5*dt, initialState + 0.5*k1, data);
	return initialState + k2;
}

}