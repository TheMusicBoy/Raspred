namespace NGuard {

////////////////////////////////////////////////////////////////////////////////

template <typename TLockable>
concept IsLockable = requires(TLockable t)
{
    { t.Lock() };
    { t.Unlock() };
};

template <typename TLockable>
requires (IsLockable<TLockable>)
class Guard {
public:
    Guard(TLockable& ptr)
        : Ptr_(ptr)
    {
        Ptr_.Lock();
    }

    ~Guard() {
        Ptr_.Unlock();
    }

private:
    TLockable& Ptr_;

};

////////////////////////////////////////////////////////////////////////////////

} // namespace NGuard
