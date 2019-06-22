module.exports = {
    mode: 'universal',
    loading: false,

    head: {
        title: 'GPU Fan Meister'
    },

    modules: [
        ['@nuxtjs/vuetify', {
            materialIcons: false,
            treeShake: true,
            iconfont: 'mdi'
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
        }],

        ['@nuxtjs/pwa', {
            workbox: false
        }],
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

    icon: {
        iconSrc: '../assets/gpu-fan-meister.png'
    }
}
