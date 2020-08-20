#include <thread>
#include <chrono>
#include "common/runtime.hpp"
#include "common/extended_window.hpp"
#include "common/dynamic_lib.hpp"
#include "switchboard_impl.hpp"
#include "common/plugin.hpp"
#include "sqlite_logger.hpp"
#include "noop_logger.hpp"
// #include "stdout_logger.hpp"

using namespace ILLIXR;

class runtime_impl : public runtime {
public:
	runtime_impl(GLXContext appGLCtx) {
		pb.register_impl<c_metric_logger>(std::make_shared<sqlite_metric_logger>());
		pb.register_impl<c_gen_guid>(std::make_shared<c_gen_guid>());
		pb.register_impl<switchboard>(create_switchboard(&pb));
		// pb.register_impl<xlib_gl_extended_window>(std::make_shared<xlib_gl_extended_window>(448*2, 320*2, appGLCtx));
		pb.register_impl<xlib_gl_extended_window>(std::make_shared<xlib_gl_extended_window>(640, 480, appGLCtx));
	}

	virtual void load_so(std::string_view so) override {
		auto lib = dynamic_lib::create(so);
		plugin_factory this_plugin_factory = lib.get<plugin* (*) (phonebook*)>("this_plugin_factory");
		load_plugin_factory(this_plugin_factory);
		libs.push_back(std::move(lib));
	}

	virtual void load_plugin_factory(plugin_factory plugin_main) override {
		plugins.emplace_back(plugin_main(&pb));
	}

	virtual void wait() override {
		while (!terminate.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds{10});
		}
	}

	virtual void stop() override {
		pb.lookup_impl<switchboard>()->stop();
		for (const std::unique_ptr<plugin>& plugin : plugins) {
			plugin->stop();
		}
		terminate.store(true);
	}

	virtual ~runtime_impl() override {
		stop();
	}

private:
	// I have to keep the dynamic libs in scope until the program is dead
	std::vector<dynamic_lib> libs;
	phonebook pb;
	std::vector<std::unique_ptr<plugin>> plugins;
	std::atomic<bool> terminate {false};
};

extern "C" runtime* runtime_factory(GLXContext appGLCtx) {
	return new runtime_impl{appGLCtx};
}
