#pragma once

#include <cstdint>
#include <vector>

using namespace std;

struct TimerHandle
{
	TimerHandle() {}
	TimerHandle(int32_t i, int32_t c) : Index(i), Counter(c) {}
	int32_t Index = -1; // no tiene nada 
	int32_t Counter = 0;

	bool operator!=(const TimerHandle Other) const { return Index != Other.Index; }

	bool IsValid() const
	{
		return *this != TimerHandle(); // es valido si no es igual a uno que se acaba de crear (valores por defecto)
	}
};

struct TimeManager
{
	TimerHandle CreateTimer(int32_t Seconds)
	{
		Counter++;
		const Timer NewTimer(Seconds, Counter); // creas un nuevo Timer pero eso no es lo que guardará el objeto que guarda los datos, cada Timer nuevo se guardará centralizado en Timers que está en esta clase 
		TimerHandle NewHandle;
		// esta busqueda podria evitarse y es mejorable (mirar FreeList data structure)
			// lo que está haceidno es pillar el primer espacio que esté vacio
		for (int32_t i = 0; i < Timers.size(); i++)
		{
			if (Timers[i].Counter == -1)
			{
				Timers[i] = NewTimer;
				NewHandle = TimerHandle(i, Counter);
				break;
			}
		}

		// si no se ha encontrado ningun espacio vacio (donde habia un elemento y luego se ha borrado) crea uno nuevo
		if (!NewHandle.IsValid())
		{
			Timers.push_back(NewTimer);
			const int32_t Index = static_cast<int32_t>(Timers.size() - 1);
			NewHandle = TimerHandle(Index, Counter);
		}

		return NewHandle;
	}

	int32_t GetTimeRemaining(const TimerHandle Timer) const
	{
		int32_t TimeRemaining = 0;
		if (Timer.IsValid())
		{
			if (Timer.Index < Timers.size())
			{
				// esto es para evitar problemas de ABA
				const bool IsSameTimer = Timer.Counter == Timers[Timer.Index].Counter;
				if (IsSameTimer)
				{
					TimeRemaining = Timers[Timer.Index].Seconds;
				}
			}
		}

		return TimeRemaining;
	}

	void DeleteTimer(TimerHandle& Timer)
	{
		if (Timer.IsValid())
		{
			if (Timer.Index < Timers.size())
			{
				const bool IsSameTimer = Timer.Counter == Timers[Timer.Index].Counter;
				if (IsSameTimer)
				{
					Timers[Timer.Index].Counter = -1;
					Timer = TimerHandle();
				}
			}
		}
	}

private:
	struct Timer
	{
		Timer(int32_t t, int32_t c) : Seconds(t), Counter(c) {}
		int32_t Seconds = 0;
		int32_t Counter = 0;
	};

	int32_t Counter = 0;
	vector<Timer> Timers;
};



