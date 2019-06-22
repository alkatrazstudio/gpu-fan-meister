<template>
<v-layout class="lang-selector">
    <v-menu bottom left>
        <v-btn flat slot="activator" class="cur-lang">
            <img-ex :src="curLang.flag" :alt="curLang.title" :height="25"/>
        </v-btn>
        <v-list>
            <v-list-tile v-for="lang in langs" :key="lang.code" :href="localePath('index', lang.code)">
                <v-list-tile-action>
                    <img-ex height="25" :src="lang.flag" class="mr-1"/>
                </v-list-tile-action>
                <v-list-tile-title>{{ lang.title }}</v-list-tile-title>
            </v-list-tile>
        </v-list>
    </v-menu>
</v-layout>
</template>

<script>
import '@mdi/font/css/materialdesignicons.css'

import {meta as enMeta} from '~/lang/en'
import {meta as ruMeta} from '~/lang/ru'

import ImgEx from '~/components/imgex'

export default {
    data() {
        return {
            langs: [
                enMeta,
                ruMeta
            ]
        }
    },
    computed: {
        curLang() {
            return this.langs.find(lang => lang.code == this.$i18n.locale)
        }
    },
    components: {
        ImgEx
    }
}
</script>

<style scoped lang="scss">
.lang-selector {
    position: absolute;
    right: 0;
    top: 0;

    .cur-lang {
        padding: 0;
        min-width: 60px;
    }
}
</style>
