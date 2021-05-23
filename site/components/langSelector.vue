<template>
<v-layout class="lang-selector">
    <v-menu bottom left>
        <template v-slot:activator="{ on }">
            <v-btn text slot="activator" class="cur-lang" v-on="on">
                <img-ex :src="curLang.flag" :alt="curLang.title" :height="25"/>
            </v-btn>
        </template>
        <v-list>
            <v-list-item v-for="lang in langs" :key="lang.code" :href="localePath('index', lang.code)">
                <v-list-item-action class="lang-switch-btn">
                    <img-ex height="25" :src="lang.flag" class="mr-1"/>
                </v-list-item-action>
                <v-list-item-title class="lang-switch-title">{{ lang.title }}</v-list-item-title>
            </v-list-item>
        </v-list>
    </v-menu>
</v-layout>
</template>

<script>
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
        width: 80px;
    }
}
</style>

<style lang="scss">
#app {
    .lang-switch-btn {
        margin-right: 10px;
        min-width: 40px;
    }

    .lang-switch-title {
        line-height: 1.5;
    }
}
</style>
