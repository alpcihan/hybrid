#pragma once

namespace hybrid {

class Time {
public:
    static float getTime() { return m_data.m_time; }
    static float getDeltaTime() { return m_data.m_deltaTime; }

private:
    class TimeData {
        private:
            float m_time = 0;
            float m_deltaTime = 0;

        private:
            void _update(float dt) {m_deltaTime = dt; m_time += dt;}

        friend class Time;
    };

    static TimeData m_data;

private:
    static void _update(float dt) {m_data._update(dt);}

    friend class Application;
};

}  // namespace hybrid