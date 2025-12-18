#pragma once
#include <vector>

struct ResourceHandle
{
	int32_t Index = -1;
	uint32_t Gen = 0;

	bool IsValid() const
	{
		return Index >= 0;
	}

	bool operator==(const ResourceHandle& other) const
	{
		return Index == other.Index && Gen == other.Gen;
	}

	bool operator!=(const ResourceHandle& other) const
	{
		return !(*this == other);
	}
};

template<typename T> 
class ResourceStorage
{
public:
	ResourceHandle Create(const T& Value)
	{
		int32_t Index;

		if (!FreeList.empty())
		{
			Index = FreeList.back();
			FreeList.pop_back();

			Entries[Index].Value = Value;
			Entries[Index].Alive = true;
			Entries[Index].Gen++;
		}
		else
		{
			Index = static_cast<int32_t>(Entries.size());
			Entries.push_back({ Value, 1, true });
		}

		return ResourceHandle{ Index, Entries[Index].Gen };
	}

	T* Get(const ResourceHandle& handle)
	{
		if (!IsValid(handle))
		{
			return nullptr;
		}
		
		return &Entries[handle.Index].Value;
	}

	void Destroy(ResourceHandle& handle)
	{
		if (!IsValid(handle))
		{
			return;
		}

		ResourceEntry& e = Entries[handle.Index];
		e.Alive = false;
		FreeList.push_back(handle.Index);

		handle = {};
	}


private:
	struct ResourceEntry
	{
		T       Value;
		int32_t Gen = 0;
		bool    Alive = false;
	};

	bool IsValid(const ResourceHandle& handle) const
	{
		if (handle.Index < 0 || handle.Index >= Entries.size())
		{
			return false;
		}

		const ResourceEntry& e = Entries[handle.Index];
		return e.Alive && e.Gen == handle.Gen;
	}

	std::vector<ResourceEntry> Entries; // pool centralizado de los recursos
	std::vector<int32_t> FreeList; // pila de indices libres porque se han eliminado lo que habia dentro
};

