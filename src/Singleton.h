//
// Created by Constantin on 19/07/2023.
//

#ifndef ENGINE_SINGLETON_H
#define ENGINE_SINGLETON_H

template<typename T>
class Singleton {
public:
    static T& instance();

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton) = delete;

protected:
    struct token {};
    Singleton() {}
};

template<typename T>
T& Singleton<T>::instance()
{
    static T instance{token{}};
    return instance;
}

#endif //ENGINE_SINGLETON_H
