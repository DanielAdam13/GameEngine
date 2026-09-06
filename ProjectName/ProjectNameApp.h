#pragma once
#include "IGameApplication.h"


namespace customProject
{
    class ProjectNameApp final : public ge::IGameApplication
    {
    public:
        ProjectNameApp() = default;
        ~ProjectNameApp() override = default;
        ProjectNameApp(const ProjectNameApp&) = delete;
        ProjectNameApp(ProjectNameApp&&) = delete;
        ProjectNameApp& operator=(const ProjectNameApp&) = delete;
        ProjectNameApp& operator=(ProjectNameApp&&) = delete;

        // ---- IGameApplication ----
        void Load() override;
        void Update(float) override;
        void FixedUpdate(float) override;

    private:
        void BindInputCommands();
    };
}
