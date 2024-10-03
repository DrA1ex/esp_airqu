import {ApplicationBase, SystemPacketType} from "./lib/index.js";

import {Config} from "./config.js";
import {Properties} from "./props.js";

export class Application extends ApplicationBase {
    #config = new Config(Properties);

    get config() {
        return this.#config;
    }

    get propertyConfig() {
        return Properties;
    }

    async begin(root) {
        await super.begin(root);

        this.propertyMeta["apply_sys_config"].control.setOnClick(this.applySysConfig.bind(this));
    }

    async applySysConfig(sender) {
        if (sender.getAttribute("data-saving") === "true") return;

        sender.setAttribute("data-saving", true);

        try {
            await this.ws.request(SystemPacketType.RESTART);

            let new_url
            if (location.hostname !== "localhost") {
                const url_parts = [
                    location.protocol + "//",
                    this.config.sys_config.mdns_name + ".local",
                    location.port ? ":" + location.port : "",
                    "/?" + (location.href.split("?")[1] ?? "")
                ]

                new_url = url_parts.join("");
            } else {
                new_url = location.href;
            }

            setTimeout(() => {
                if (window.location === new_url) {
                    window.location.reload();
                } else {
                    window.location = new_url;
                }
            }, 3000);
        } catch (err) {
            console.log("Unable to send restart signal", err);
            sender.setAttribute("data-saving", false);
        }
    }
}