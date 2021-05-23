module.exports = {
    loading: false,
    target: 'static',
    telemetry: false,

    head: {
        title: 'GPU Fan Meister'
    },

    modules: [
        ['@nuxtjs/vuetify', {
            treeShake: true,
            defaultAssets: false,
            optionsPath: './vuetify.options.js'
        }],

        ['nuxt-i18n', {
            locales: [
                { code: 'en', iso: 'en-US', file: 'en.js' },
                { code: 'ru', iso: 'ru-RU', file: 'ru.js' },
            ],
            defaultLocale: 'en',
            lazy: true,
            langDir: 'lang/',
            vueI18n: {
                fallbackLocale: 'en'
            }
        }]
    ],

    build: {
        extractCSS: process.env.NODE_ENV == 'production',
        transpile: ['svg-country-flags/svg'],
        loaders: {
            vue: {
                transformAssetUrls: {
                    'v-img': 'src',
                    'img': 'src',
                    'img-ex': 'src'
                },
                compilerOptions: {
                    whitespace: 'condense'
                }
            }
        }
    },

    generate: {
        exclude: [
            /^\/(ru\/)?doc/
        ]
    },

    icon: {
        iconSrc: '../assets/gpu-fan-meister.png'
    }
}
